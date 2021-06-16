//
// Created by igor on 13/07/2020.
//

#include "plugin/processors/basic_processor.hh"

#include "plugin/logger.hh"
#include <libecap/host/xaction.h>
#include <libecap/common/message.h>
#include <libecap/common/header.h>
#include <libecap/common/name.h>
#include <libecap/common/names.h>
#include <libecap/common/errors.h>

#include "plugin/processors/perf_counter.hh"
#include "backend/unused.hh"

namespace binadox
{
    basic_processor::basic_processor(libecap::host::Xaction* x)
            : hostx(x),
              receivingVb(opUndecided),
              sendingAb(opUndecided),
              processed(eNONE),
              consumer(nullptr)
    {
    }
    // ----------------------------------------------------------------------------------------
    basic_processor::~basic_processor()
    {
        if (libecap::host::Xaction* x = hostx)
        {
            hostx = 0;
            x->adaptationAborted();
        }
        delete consumer;
    }
    // ----------------------------------------------------------------------------------------
    const libecap::Area basic_processor::option(const libecap::Name&) const
    {
        return libecap::Area(); // this transaction has no meta-information
    }
    // ----------------------------------------------------------------------------------------------------
    // this transaction has no meta-information
    void basic_processor::visitEachOption(libecap::NamedValueVisitor&) const
    {
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::start()
    {

        ENFORCE(hostx);
        libecap::FirstLine* firstLine = &(hostx->virgin().firstLine());
        ENFORCE(firstLine);

        processed = eNONE;
        timer.start();
        nonstd::string_view domain;
        if (libecap::RequestLine* requestLine = dynamic_cast<libecap::RequestLine*>(firstLine))
        {
            const auto& method = requestLine->method();
            if (method != libecap::methodConnect &&
                method != libecap::methodHead &&
                method != libecap::methodOptions &&
                method != libecap::methodTrace)
            {
                const libecap::Area& uri = requestLine->uri();

                if (get_config()->accept_url(uri, domain))
                {
                    url = std::string(uri.start, uri.size);
                    if (on_start_request(requestLine, domain))
                    {
                        processed = eREQUEST;
                    }
                }
            }
        }
        if (libecap::StatusLine* statusLine = dynamic_cast<libecap::StatusLine*>(firstLine))
        {
            if (const libecap::RequestLine* requestLine = dynamic_cast<const libecap::RequestLine*>(&hostx->cause().firstLine()))
            {
                const libecap::Area& uri = requestLine->uri();

                if (get_config()->accept_url(uri, domain))
                {
                    url = std::string(uri.start, uri.size);

                    if (on_start_response(statusLine, requestLine, domain))
                    {
                        processed = eRESPONSE;
                    }
                }
            }
        }
        if (processed == eNONE)
        {
            sendingAb = opNever;
            lastHostCall()->useVirgin();
        } else
        {
            if (adapted)
            {
                forge_headers(adapted);
            }

            if (receivingVb == opOn)
            {
                hostx->vbMake();
            } else {
                if (receivingVb == opNever)
                {
                    if (adapted)
                    {
                        hostx->useAdapted(adapted);
                    }
                }
            }

            if (processed == eRESPONSE)
            {
                LOG_TRACE("RESPONSE ", url);
            }
            if (processed == eREQUEST)
            {
                LOG_TRACE("REQUEST ", url);
            }
        }

    }
    // ----------------------------------------------------------------------------------------------------
    bool basic_processor::on_start_request(UNUSED_ARG const libecap::RequestLine*, UNUSED_ARG const nonstd::string_view&)
    {
        return false;
    }
    // ----------------------------------------------------------------------------------------------------
    bool basic_processor::on_start_response(UNUSED_ARG const libecap::StatusLine*,
                                            UNUSED_ARG const libecap::RequestLine* requestLine,
                                            UNUSED_ARG const nonstd::string_view& domain)
    {
        return false;
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::on_stop()
    {

    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::stop()
    {
        hostx = nullptr;
        // the caller will delete
        on_stop();

        if (consumer)
        {
            delete consumer;
            consumer = nullptr;
        }

        if (processed != eNONE)
        {
            if (!url.empty() && get_config())
            {
                double us = timer.get_elapsed_us();
                get_config()->get_perf_counter().add(url, us, processed);
            }
        }
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::abDiscard()
    {
        ENFORCE(sendingAb == opUndecided); // have not started yet
        sendingAb = opNever;
        // we do not need more vb if the host is not interested in ab
        stopVb();
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::abMake()
    {
        ENFORCE(sendingAb == opUndecided); // have not yet started or decided not to send

        sendingAb = opOn;
        hostx->noteAbContentAvailable();
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::abMakeMore()
    {
        ENFORCE(receivingVb == opOn); // a precondition for receiving more vb
        hostx->vbMakeMore();
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::abStopMaking()
    {
        sendingAb = opComplete;
        // we do not need more vb if the host is not interested in more ab
        stopVb();
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::stopVb()
    {
        if (receivingVb == opOn)
        {
            hostx->vbStopMaking(); // we will not call vbContent() any more
            receivingVb = opComplete;
        } else
        {
            // we already got the entire body or refused it earlier
            ENFORCE(receivingVb != opUndecided);
        }
    }
    // ----------------------------------------------------------------------------------------------------
    // this method is used to make the last call to hostx transaction
    // last call may delete adapter transaction if the host no longer needs it
    // TODO: replace with hostx-independent "done" method
    libecap::host::Xaction* basic_processor::lastHostCall()
    {
        libecap::host::Xaction* x = hostx;
        ENFORCE(x);
        hostx = nullptr;
        return x;
    }
    // ----------------------------------------------------------------------------------------------------
    void basic_processor::set_consumer(pipeline* the_consumer)
    {
        consumer = the_consumer;
    }
    // ----------------------------------------------------------------------------------------------------
    // adapted body content extraction and consumption
    libecap::Area basic_processor::abContent(libecap::size_type offset, libecap::size_type size)
    {
        ENFORCE(consumer);
        ENFORCE(sendingAb == opOn || sendingAb == opComplete);
        libecap::Area ret = consumer->transmit(offset, size);
        return ret;
    }
    // -------------------------------------------------------------------------------------------------------------
    void basic_processor::abContentShift(libecap::size_type size)
    {
        consumer->update_transmitted_offset(size);

        if (consumer->is_finished())
        {
            if (sendingAb == opOn)
            {
                sendingAb = opComplete;
                hostx->noteAbContentDone(true);
            }
        }

        if (receivingVb == opOn)
        {
            hostx->vbContentShift(consumer->get_last_input_length());
        }
    }
    // -------------------------------------------------------------------------------------------------------------
    // virgin body state notification
    void basic_processor::noteVbContentDone(UNUSED_ARG bool atEnd)
    {
        ENFORCE(consumer);
        consumer->finish();

        ENFORCE(receivingVb == opOn);

        stopVb();

        if (sendingAb == opUndecided)
        {
            hostx->useAdapted(adapted);
        }
        if (sendingAb == opOn)
        {
            if (consumer->is_finished())
            {
                sendingAb = opComplete;
                hostx->noteAbContentDone(true);
            }
        }
        hostx->noteAbContentAvailable();
    }
    // -------------------------------------------------------------------------------------------------------------
    void basic_processor::noteVbContentAvailable()
    {
        ENFORCE(consumer);
        const libecap::Area vb = hostx->vbContent(0, 32768);
        hostx->vbContentShift(vb.size); // we have a copy; do not need vb any more

        consumer->consume(vb.start, vb.size);

        if (sendingAb == opUndecided)
        {
            hostx->useAdapted(adapted);
        }
        else
        {
            hostx->noteAbContentAvailable();
        }
    }
    // -------------------------------------------------------------------------------------------------------------
    void basic_processor::set_adapted(libecap::shared_ptr<libecap::Message> msg)
    {
        adapted = msg;
    }
    // -------------------------------------------------------------------------------------------------------------
    void basic_processor::forge_headers(UNUSED_ARG libecap::shared_ptr<libecap::Message> msg)
    {

    }
} // ns binadox