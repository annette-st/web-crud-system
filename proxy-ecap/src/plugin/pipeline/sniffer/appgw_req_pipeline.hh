//
// Created by igor on 11/01/2021.
//

#ifndef BINADOX_ECAP_APPGW_REQ_PIPELINE_HH
#define BINADOX_ECAP_APPGW_REQ_PIPELINE_HH


#include "plugin/pipeline/sniffer/basic_sniffer.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"
#include "appgw/addon_token.hh"

namespace binadox
{
    class appgw_req_pipeline : public basic_sniffer
    {
    public:
        appgw_req_pipeline ();
        ~appgw_req_pipeline();

        static bool enabled();

        static appgw::addon_token accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        static bool wants_url(const char* url, std::size_t url_len);

        virtual bool init(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        virtual libecap::shared_ptr<libecap::Message> generate_response();
    private:
        libecap::shared_ptr<libecap::Message> adapted;
    };
} // ns binadox



#endif //BINADOX_ECAP_APPGW_REQ_PIPELINE_HH
