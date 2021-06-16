//
// Created by igor on 13/07/2020.
//

#include <memory>
#include <string.h>

#include <libecap/host/xaction.h>
#include <libecap/common/errors.h>
#include <libecap/common/message.h>
#include <libecap/common/header.h>
#include <libecap/common/names.h>

#include <nonstd/string_view.hpp>

#include "plugin/logger.hh"
#include "plugin/constants.hh"
#include "plugin/processors/request_processor.hh"
#include "plugin/pipeline/script_ipc_pipeline.hh"
#include "plugin/pipeline/blacklist_pipeline.hh"
#include "plugin/pipeline/sniffer/sniffer_req_pipeline.hh"
#include "plugin/pipeline/sniffer/appgw_req_pipeline.hh"
#include "backend/unused.hh"


// curl -k --proxy http://127.0.0.1:3128 'http://example.com/3f785034-d25a-11ea-87d0-0242ac130003' -H 'User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:79.0) Gecko/20100101 Firefox/79.0' -H 'Accept: */*' -H 'Accept-Language: en-US,en;q=0.5' --compressed -H 'Content-Type: application/x-www-form-urlencoded' -H 'Origin: http://example.com' -H 'Connection: keep-alive' -H 'Referer: http://example.com/' --data-raw '{"message_type":"ping","addr":"127.0.0.1","tabId":"8a0c0f26-51bb-c78f-c912-0ce4127892a5","GUID":"123456","orig":"aHR0cDovL2V4YW1wbGUuY29t","url":"aHR0cDovL2V4YW1wbGUuY29tLw=="}'

namespace binadox
{
    typedef bool (*wants_url_t)(const char* url, std::size_t len);
    wants_url_t wants_url_func [] = {
            &script_ipc_pipeline::wants_url,
            &blacklist_pipeline::wants_url,
            &sniffer_req_pipeline::wants_url,
            &appgw_req_pipeline::wants_url,
            nullptr
    };

    request_processor::factory_t request_processor::factory_methods[] = {
            &request_processor::create_ipc_pipeline,
            &request_processor::create_blacklist_pipeline,
            &request_processor::create_sniffer_pipeline,
            nullptr
    };
    // ==================================================================================================
    request_processor::pipeline_props::pipeline_props()
    : need_vb(false),
      message(),
      consumer(nullptr)
    {

    }
    // ---------------------------------------------------------------------------------------------------
    request_processor::pipeline_props::pipeline_props(std::unique_ptr<pipeline>&& handler, libecap::shared_ptr<libecap::Message> ab, bool vb)
    : need_vb(vb),
    message(ab),
    consumer(handler.release())
    {

    }
    // ==================================================================================================
    request_processor::request_processor(libecap::host::Xaction *x)
    : basic_processor(x)
    {
    }
    // -------------------------------------------------------------------------------------------------------
    request_processor::~request_processor()
    {
    }
    // -------------------------------------------------------------------------------------------------------
    request_processor::pipeline_props request_processor::create_ipc_pipeline(const libecap::RequestLine* line, UNUSED_ARG const nonstd::string_view& domain)
    {
        if (!script_ipc_pipeline::accept(hostx, line))
        {
            return request_processor::pipeline_props();
        }

        libecap::shared_ptr<libecap::Message> orig = hostx->virgin().clone();

        if (!orig->body())
        {
            return request_processor::pipeline_props();
        }

        std::unique_ptr<request_pipeline> handler  (new script_ipc_pipeline());
        if (handler->init(hostx, line))
        {
            libecap::shared_ptr<libecap::Message> adapted = handler->generate_response();
            return pipeline_props(std::move(handler), adapted, true);
        }
        return pipeline_props();
    }
    // -------------------------------------------------------------------------------------------------------
    request_processor::pipeline_props request_processor::create_blacklist_pipeline(const libecap::RequestLine* line, UNUSED_ARG const nonstd::string_view& domain)
    {
        if (!blacklist_pipeline::accept(hostx, line))
        {
            return pipeline_props();
        }

        std::unique_ptr<request_pipeline> handler (new blacklist_pipeline());
        if (handler->init(hostx, line))
        {
            libecap::shared_ptr<libecap::Message> adapted = handler->generate_response();
            return pipeline_props(std::move(handler), adapted, false);
        }
        return pipeline_props();
    }
    // -------------------------------------------------------------------------------------------------------
    request_processor::pipeline_props request_processor::create_sniffer_pipeline(const libecap::RequestLine* line, UNUSED_ARG const nonstd::string_view& domain)
    {
        bool initialized = false;
        std::unique_ptr<basic_sniffer> handler;
        if (sniffer_req_pipeline::accept(hostx, line))
        {
            auto* sniffer =  new sniffer_req_pipeline();
            handler = std::unique_ptr<basic_sniffer>(sniffer);
            initialized = sniffer->init(hostx, line);

        } else
        {
            if (appgw_req_pipeline::enabled())
            {
                auto tk = appgw_req_pipeline::accept(hostx, line);
                if (!tk.accepted().empty())
                {
                    auto* sniffer = new appgw_req_pipeline();
                    handler = std::unique_ptr<basic_sniffer>(sniffer);
                    initialized = sniffer->init(hostx, line);
                    if (initialized)
                    {
                        sniffer->get_meta_data()->set_script(tk.accepted());
                    }
                }
            }
        }

        if (initialized)
        {
            libecap::shared_ptr<libecap::Message> adapted = handler->generate_response();
            if (adapted->body())
            {
                return pipeline_props(std::move(handler), adapted, true);
            } else
            {
                handler->finish();
                return pipeline_props(std::move(handler), adapted, false);
            }
        }
        return pipeline_props();
    }
    // -------------------------------------------------------------------------------------------------------
    bool request_processor::on_start_request(const libecap::RequestLine* line, UNUSED_ARG const nonstd::string_view& domain)
    {
        if (hostx->virgin().header().hasAny(binadoxMark))
        {
            return false;
        }

        int k = 0;
        while (true)
        {
            factory_t method = factory_methods[k++];
            if (!method)
            {
                break;
            }

            pipeline_props rc = (this->*method)(line, domain);
            if (rc.consumer)
            {
                set_consumer(rc.consumer);
                if (rc.need_vb)
                {
                    receivingVb = opOn;
                } else
                {
                    receivingVb = opNever;

                }
                if (rc.message)
                {
                    set_adapted(rc.message);
                }
                return true;
            }
        }

        return false;
    }
    // -------------------------------------------------------------------------------------------------------
    bool request_processor::wants_url(const char* url)
    {
        if (!url)
        {
            return false;
        }
        size_t len = strlen(url);
        std::size_t k = 0;
        while(true)
        {
            wants_url_t func = wants_url_func[k];
            if (!func)
            {
                break;
            }
            k++;
            if (func(url, len))
            {
                return true;
            }
        }
        return false;
    }
    // ---------------------------------------------------------------------------------------------------------
} // ns binadox