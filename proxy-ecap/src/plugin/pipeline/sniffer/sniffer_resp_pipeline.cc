//
// Created by igor on 29/08/2020.
//

#include <libecap/host/xaction.h>

#include "plugin/pipeline/sniffer/sniffer_resp_pipeline.hh"
#include "plugin/constants.hh"

namespace binadox
{

    sniffer_resp_pipeline::sniffer_resp_pipeline()
    {

    }
    // ---------------------------------------------------------------------------------------------
    sniffer_resp_pipeline::~sniffer_resp_pipeline()
    {

    }
    // ---------------------------------------------------------------------------------------------
    bool sniffer_resp_pipeline::init(libecap::host::Xaction* hostx,
                                     const libecap::StatusLine* line,
                                     const libecap::Area& trid)
    {
        if (!trid.size)
        {
            return false;
        }

        adapted = hostx->virgin().clone();
        if (!adapted)
        {
            return false;
        }

        libecap::Area ip = hostx->option(headerXclientip);

        sniffer_meta_data* data = new sniffer_meta_data(line);
        data->set_client_ip(ip.toString());
        data->load_headers(adapted);
        data->set_transaction_id(trid.toString());

        use_sniffer_metadata(data);

        if (!adapted->body())
        {
            return false;
        }
        // filter by content type
        return true;
    }
    // ---------------------------------------------------------------------------------------------
    libecap::shared_ptr <libecap::Message> sniffer_resp_pipeline::generate_response()
    {
        return adapted;
    }

} // ns binadox
