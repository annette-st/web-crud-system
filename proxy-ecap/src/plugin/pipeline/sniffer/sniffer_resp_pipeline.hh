//
// Created by igor on 29/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_RESP_PIPELINE_HH
#define BINADOX_ECAP_SNIFFER_RESP_PIPELINE_HH

#include "plugin/pipeline/sniffer/basic_sniffer.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"

namespace binadox
{
    class sniffer_resp_pipeline : public basic_sniffer
    {
    public:
        sniffer_resp_pipeline();
        ~sniffer_resp_pipeline();

        virtual bool init(libecap::host::Xaction* hostx,
                          const libecap::StatusLine* line,
                          const libecap::Area& trid);



        virtual libecap::shared_ptr <libecap::Message> generate_response();
    private:
        libecap::shared_ptr <libecap::Message> adapted;
    };

} // ns binadox


#endif //BINADOX_ECAP_SNIFFER_RESP_PIPELINE_HH
