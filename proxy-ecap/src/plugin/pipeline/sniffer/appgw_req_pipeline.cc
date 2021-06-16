//
// Created by igor on 11/01/2021.
//

#include "appgw_req_pipeline.hh"
//
// Created by igor on 23/08/2020.
//

#include <string.h>
#include <libecap/host/xaction.h>
#include <libecap/common/named_values.h>

#include "plugin/conf/configurator.hh"
#include "plugin/fast_rand.hh"
#include "plugin/constants.hh"
#include "appgw/addons_registry.hh"
#include "backend/unused.hh"


namespace binadox
{
    struct headers_builder : public libecap::NamedValueVisitor
    {
        std::map<std::string, std::string> headers;

        void visit(const libecap::Name& name, const libecap::Area& value)
        {
            headers[name.image()] = value.toString();
        }
    };
    // ===========================================================================================================
    appgw_req_pipeline::appgw_req_pipeline ()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    appgw_req_pipeline::~appgw_req_pipeline()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    appgw::addon_token appgw_req_pipeline::accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
        nonstd::string_view url (line->uri().start, line->uri().size);
        auto tk = get_config()->addons_registry().wants_url(url);
        if (tk.wants_url())
        {
            nonstd::string_view method = line->method().image();
            headers_builder hb;
            const auto& virgin_headers = hostx->virgin().header();
            virgin_headers.visitEach(hb);
            if (get_config()->addons_registry().accept(tk, method, url, hb.headers))
            {
                return tk;
            }
        }
        return appgw::addon_token();
    }
    // -----------------------------------------------------------------------------------------------------------
    bool appgw_req_pipeline::enabled()
    {
        return !get_config()->addons_registry().empty();
    }
    // -----------------------------------------------------------------------------------------------------------
    bool appgw_req_pipeline::wants_url(UNUSED_ARG const char* url, UNUSED_ARG std::size_t url_len)
    {
        return enabled();
    }
    // -----------------------------------------------------------------------------------------------------------
    bool appgw_req_pipeline::init(libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
        adapted = hostx->virgin().clone();
        if (!adapted)
        {
            return false;
        }

        libecap::Area ip = hostx->option(headerXclientip);

        auto* data = new sniffer_meta_data(line);
        data->set_client_ip(ip.toString());

#define TRID_LEN 16
        char trid[TRID_LEN+1];
        trid[TRID_LEN] = 0;
        gen_random_string(trid, TRID_LEN);
        trid[0] = '_';

        adapted->header().add(transactionID, libecap::Area::FromTempString(trid));
        data->load_headers(adapted);
        use_sniffer_metadata(data);

        return true;
    }
    // -----------------------------------------------------------------------------------------------------------
    libecap::shared_ptr<libecap::Message> appgw_req_pipeline::generate_response()
    {
        return adapted;
    }

} // ns binadox
