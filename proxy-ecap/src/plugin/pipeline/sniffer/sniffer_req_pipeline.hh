//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_REQ_PIPELINE_HH
#define BINADOX_ECAP_SNIFFER_REQ_PIPELINE_HH

#include "plugin/pipeline/sniffer/basic_sniffer.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"

namespace binadox
{
    class sniffer_req_pipeline : public basic_sniffer
    {
    public:
        sniffer_req_pipeline ();
        ~sniffer_req_pipeline();

        static bool accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        static bool wants_url(const char* url, std::size_t url_len);

        virtual bool init(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        virtual libecap::shared_ptr<libecap::Message> generate_response();
    private:
        libecap::shared_ptr<libecap::Message> adapted;
    };
} // ns binadox

#endif //BINADOX_ECAP_SNIFFER_REQ_PIPELINE_HH
