//
// Created by igor on 23/08/2020.
//

#include <string.h>
#include <libecap/host/xaction.h>
#include "plugin/pipeline/sniffer/sniffer_req_pipeline.hh"
#include "plugin/conf/configurator.hh"
#include "plugin/fast_rand.hh"
#include "plugin/constants.hh"
#include "backend/unused.hh"


namespace binadox
{
    sniffer_req_pipeline::sniffer_req_pipeline ()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    sniffer_req_pipeline::~sniffer_req_pipeline()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    bool sniffer_req_pipeline::accept(libecap::host::Xaction *hostx, UNUSED_ARG const libecap::RequestLine* line)
    {
        libecap::Area ip = hostx->option(headerXclientip);
        return (get_config()->need_to_sniff_from_ip(ip));
    }
    // -----------------------------------------------------------------------------------------------------------
    bool sniffer_req_pipeline::wants_url(UNUSED_ARG const char* url, UNUSED_ARG std::size_t url_len)
    {
        return get_config()->is_sniffer_available();
    }
    // -----------------------------------------------------------------------------------------------------------
    bool sniffer_req_pipeline::init(libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
        adapted = hostx->virgin().clone();
        if (!adapted)
        {
            return false;
        }

        libecap::Area ip = hostx->option(headerXclientip);

        sniffer_meta_data* data = new sniffer_meta_data(line);
        data->set_client_ip(ip.toString());

#define TRID_LEN 16
        char trid[TRID_LEN+1];
        trid[TRID_LEN] = 0;
        gen_random_string(trid, TRID_LEN);

        adapted->header().add(transactionID, libecap::Area::FromTempString(trid));
        data->load_headers(adapted);
        use_sniffer_metadata(data);

        // filter by content type

        return true;
    }
    // -----------------------------------------------------------------------------------------------------------
    libecap::shared_ptr<libecap::Message> sniffer_req_pipeline::generate_response()
    {
        return adapted;
    }

} // ns binadox
