//
// Created by igor on 13/07/2020.
//

#include "plugin/processors/response_processor.hh"

#include "plugin/logger.hh"
#include "plugin/constants.hh"
#include "plugin/pipeline/response_pipeline.hh"
#include "plugin/pipeline/response_replace_pipeline.hh"
#include "plugin/pipeline/sniffer/sniffer_resp_pipeline.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"
#include "plugin/sniffer_db/sniffer_db.hh"
#include "plugin/filter/response_acceptor.hh"
#include "plugin/filter/replace_engine.hh"

#include <libecap/common/errors.h>
#include <libecap/host/xaction.h>
#include <libecap/common/message.h>
#include <libecap/common/header.h>
#include <libecap/common/version.h>

namespace binadox
{
    response_processor::response_processor(libecap::host::Xaction* x)
            : basic_processor(x),
              csp(nullptr),
              csp_report_only(nullptr),
              sniffer_md(nullptr),
              sniffer_only(false)
    {

    }
    // -------------------------------------------------------------------------------------------------------------
    response_processor::~response_processor()
    {

    }
    // -------------------------------------------------------------------------------------------------------------
    bool response_processor::on_start_response(const libecap::StatusLine* line,
                                               const libecap::RequestLine* requestLine,
                                               const nonstd::string_view& domain)
    {
        const libecap::Area& trid = hostx->cause().header().value(transactionID);
        const libecap::Area& origin = hostx->cause().header().value(addonOriginDomain);

        pipeline* handler = create_sniffer_pipeline(line, trid);
        if (handler)
        {
            set_consumer(handler);
            sniffer_only = true;
            return true;
        }
        handler = create_replacement_pipeline(line, requestLine);
        if (handler) {
            set_consumer(handler);
            return true;
        }
        handler = create_transformer_pipeline(line, requestLine, domain, trid, origin);
        if (handler)
        {
            set_consumer(handler);
            return true;
        }
        return false;
    }
    // -------------------------------------------------------------------------------------------------------------
    pipeline* response_processor::create_replacement_pipeline(const libecap::StatusLine* line,
                                                              const libecap::RequestLine* requestLine)
    {
        if (get_config()->get_replace_engine().empty() || requestLine->uri().size == 0) {
            return nullptr;
        }

        if (line->statusCode() != 200)
        {
            return nullptr;
        }

        if (requestLine->method() != libecap::methodGet)
        {
            return nullptr;
        }

        if (hostx->virgin().body())
        {
            receivingVb = opOn;
        } else
        {
            return nullptr;
        }

        std::string text = get_config()->get_replace_engine().find(requestLine->uri().toString());
        if (text.empty()) {
            return nullptr;
        }
        auto msg = hostx->virgin().clone();
        ENFORCE(msg != 0);

        if (!msg->body())
        {
            return nullptr;
        }

        pipeline* handler = new response_replace_pipeline(std::move(text));
        handler->update_headers(msg);
        set_adapted(msg);

        return handler;
    }
    // -------------------------------------------------------------------------------------------------------------
    pipeline* response_processor::create_transformer_pipeline(const libecap::StatusLine* line,
                                                              const libecap::RequestLine* requestLine,
                                                              const nonstd::string_view& domain,
                                                              const libecap::Area& trid,
                                                              const libecap::Area& origin)
    {
        if (line->statusCode() != 200)
        {
            return nullptr;
        }

        if (requestLine->method() != libecap::methodGet)
        {
            return nullptr;
        }

        libecap::Area ip = hostx->option(headerXclientip);

        if (ip.size == 0)
        {
            return nullptr;
        }

        if (hostx->virgin().body())
        {
            receivingVb = opOn;
        } else
        {
            return nullptr;
        }

        origin_ip = ip.toString();
        response_acceptor filter;
        const auto& virgin_headers = hostx->virgin().header();
        virgin_headers.visitEach(filter);


        if (!filter.valid())
        {
            return nullptr;
        }

        auto msg = hostx->virgin().clone();
        ENFORCE(msg != nullptr);

        if (!msg->body())
        {
            return nullptr;
        }

        csp = filter.acquire_csp_header();
        csp_report_only = filter.acquire_csp_header_report_only();

        binadox::pipeline_context ctx (filter.encoding(), csp, csp_report_only);

        ctx.access_allow_origin = filter.get_access_allow_origin();
        ctx.addon_origin = origin.toString();
        ctx.xframeoptions = filter.get_xframe_options();
        ctx.xxssprotection = filter.get_xssprotection();
        if (trid.size > 0)
        {
            sniffer_md = new sniffer_meta_data(line);
            sniffer_md->set_client_ip(origin_ip);
            sniffer_md->load_headers(msg);
            sniffer_md->set_transaction_id(trid.toString());
            ctx.sniffer_md = sniffer_md;
            LOG_TRACE("Start transaction ", sniffer_md->get_transaction_id());
            get_config()->sniffer_database().put(*sniffer_md);
        }

        ctx.origin_ip = origin_ip;
        ctx.destination_url = domain.to_string();

        response_pipeline* handler = new response_pipeline(ctx);
        handler->update_headers(msg);
        set_adapted(msg);

        return handler;
    }
    // -------------------------------------------------------------------------------------------------------------
    pipeline* response_processor::create_sniffer_pipeline(const libecap::StatusLine* line, const libecap::Area& trid)
    {
        if (!trid.size || !line)
        {
            return nullptr;
        }

        if (hostx->virgin().body())
        {
            receivingVb = opOn;
        }
        std::unique_ptr<sniffer_resp_pipeline> handler = std::unique_ptr<sniffer_resp_pipeline>(new sniffer_resp_pipeline());
        if (handler->init(hostx, line, trid))
        {
            set_adapted(handler->generate_response());
            return handler.release();
        }
        return nullptr;
    }
    // -------------------------------------------------------------------------------------------------------------
    void response_processor::on_stop()
    {
        delete csp;
        delete csp_report_only;
        delete sniffer_md;
    }
    // -------------------------------------------------------------------------------------------------------------
    void response_processor::forge_headers(libecap::shared_ptr<libecap::Message> msg)
    {
        if (!sniffer_only)
        {
            static const libecap::Name headerBinadox("X-BINADOX");
            msg->header().add(headerBinadox, libecap::Area::FromTempString("1"));
        }
    }
} // ns binadox
