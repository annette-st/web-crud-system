//
// Created by igor on 13/08/2020.
//

#include <string.h>
#include <vector>
#include <libecap/host/xaction.h>
#include <libecap/host/host.h>
#include <libecap/common/autoconf.h>
#include <libecap/common/registry.h>

#include "plugin/pipeline/script_ipc_pipeline.hh"
#include "plugin/constants.hh"
#include "plugin/conf/key_value_storage.hh"
#include "backend/unused.hh"

#include <nlohmann/json.hpp>

namespace
{
    struct not_found
    {
        not_found()
        {
            nlohmann::json obj = nlohmann::json::object();
            obj["error"] = "not found";
            txt = obj.dump();
        }

        std::string txt;
    };
}

namespace binadox
{
    class ipc_mapper
    {
    public:
        ipc_mapper()
        {
            pairs.push_back(pair_t("/login", ipc::eLOGIN));
            pairs.push_back(pair_t("/store", ipc::eKEY_VALUE));
            pairs.push_back(pair_t("/event", ipc::eTRACKING_DATA));
            pairs.push_back(pair_t("/events", ipc::eTRACKING_DATA));
        }
        ipc::message_type_t map(const libecap::RequestLine* line) const
        {
            for (std::size_t i=0; i<pairs.size(); i++)
            {
                const pair_t & p = pairs[i];
                size_t lenstr = line->uri().size;
                size_t lensuffix = p.first.size();
                if (lensuffix >  lenstr)
                {
                    continue;
                }
                if (strncmp(line->uri().start + lenstr - lensuffix, p.first.c_str(), lensuffix) == 0)
                {
                    return p.second;
                }
            }
            return ipc::eTRACKING_DATA;
        }
    private:
        typedef std::pair<std::string, ipc::message_type_t> pair_t;
        std::vector<pair_t> pairs;
    };
    // =========================================================================================================
    script_ipc_pipeline::script_ipc_pipeline ()
    : message_type(ipc::eTRACKING_DATA),
      sending_offset(0)
    {

    }
    // --------------------------------------------------------------------------------------------------------
    script_ipc_pipeline::~script_ipc_pipeline()
    {

    }
    // --------------------------------------------------------------------------------------------------------
    bool script_ipc_pipeline::accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
        if (!line)
        {
            return false;
        }
        if (line->method() != libecap::methodPost)
        {
            return false;
        }
        libecap::shared_ptr<libecap::Message> orig = hostx->virgin().clone();

        if (!orig || !orig->body())
        {
            return false;
        }
        if (line->uri().size < acceptor_uuid.size())
        {
            return false;
        }
        if (strstr(line->uri().start, acceptor_uuid.c_str()))
        {
            return true;
        }
        return false;
    }
    // --------------------------------------------------------------------------------------------------------
    bool script_ipc_pipeline::init(libecap::host::Xaction * /*hostx*/, const libecap::RequestLine* line)
    {
        static  ipc_mapper mapper;
        message_type = mapper.map(line);
        return true;
    }
    // --------------------------------------------------------------------------------------------------------
    libecap::shared_ptr<libecap::Message> script_ipc_pipeline::generate_response()
    {
        // use ECAP content satisfaction to forge the message
        adapted = libecap::MyHost().newResponse();
        libecap::StatusLine &statusLine = dynamic_cast<libecap::StatusLine&>(adapted->firstLine());
        statusLine.statusCode(200);
        statusLine.reasonPhrase(libecap::Area::FromTempString("OK"));
        adapted->header().add(headerContentType, libecap::Area::FromTempString("application/json"));
        adapted->addBody();

        return adapted;
    }
    // --------------------------------------------------------------------------------------------------------
    bool script_ipc_pipeline::wants_url(const char* url, std::size_t url_len)
    {
        if (1 + url_len < acceptor_uuid.size())
        {
            return false;
        }
        std::size_t ptr = (url[0] == '/' ? 1 : 0);
        return strncmp(url + ptr, acceptor_uuid.c_str(), acceptor_uuid.size()) == 0;
    }
    // --------------------------------------------------------------------------------------------------------
    void script_ipc_pipeline::consume(const char* data, std::size_t size)
    {
        if (!input)
        {
            input = std::make_shared<std::string>();
        }

        *input += std::string(data, data + size);
    }
    // --------------------------------------------------------------------------------------------------------
    void script_ipc_pipeline::finish ()
    {
        if (get_config())
        {
            if (message_type != ipc::eKEY_VALUE)
            {
                get_config()->get_ipc()->send(message_type, input);
                input = std::make_shared<std::string>();
            } else
            {
                auto message = nlohmann::json::parse(*input);
                if (!message.contains("domain")) {
                    return;
                }
                std::string domain = message["domain"];
                std::string operation = "";
                if (message.contains("operation"))
                {
                    operation = message["operation"];
                }
                if (operation.empty() || operation == "put")
                {
                    if (!message.contains("payload")) {
                        return;
                    }
                    auto payload = message["payload"].dump();

                    get_config()->get_key_value_storage().put(domain, audit_config, payload);
                } else {
                    if (operation == "get")
                    {
                        if (!get_config()->get_key_value_storage().get_with_meta(domain, audit_config, sending_buffer)) {
                            static not_found err;
                            sending_buffer = err.txt;
                        }
                    } else {
                        if (operation == "delete") {
                            get_config()->get_key_value_storage().drop(domain, audit_config);
                        }
                    }
                }
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------
    libecap::Area script_ipc_pipeline::transmit(libecap::size_type offs, UNUSED_ARG libecap::size_type size)
    {
        const std::size_t offset = sending_offset + offs;
        const std::size_t to_transmit = sending_buffer.size() - offset;
        if (offset >= sending_buffer.size())
        {
            return libecap::Area();
        }
        return libecap::Area::FromTempBuffer(sending_buffer.c_str() + offset, to_transmit);
    }
    // ---------------------------------------------------------------------------------------------------
    void script_ipc_pipeline::update_transmitted_offset (std::size_t offs)
    {
        sending_offset += offs;
    }
    // ---------------------------------------------------------------------------------------------------

} // ns binadox

