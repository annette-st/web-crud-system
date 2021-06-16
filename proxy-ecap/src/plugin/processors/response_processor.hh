//
// Created by igor on 13/07/2020.
//

#ifndef BINADOX_ECAP_RESPONSE_PROCESSOR_HH
#define BINADOX_ECAP_RESPONSE_PROCESSOR_HH

#include "plugin/processors/basic_processor.hh"
#include "plugin/pipeline/pipeline.hh"

namespace binadox
{
    class csp_header;
    class sniffer_meta_data;

    class response_processor : public basic_processor
    {
    public:
        response_processor(libecap::host::Xaction* x);
        virtual ~response_processor();
    private:
        virtual bool on_start_response(const libecap::StatusLine* line, const libecap::RequestLine* requestLine,
                                       const nonstd::string_view& domain);
        virtual void on_stop();
        virtual void forge_headers(libecap::shared_ptr<libecap::Message> msg);
    private:
        pipeline* create_transformer_pipeline(const libecap::StatusLine* line,
                                              const libecap::RequestLine* requestLine,
                                              const nonstd::string_view& domain,
                                              const libecap::Area& trid,
                                              const libecap::Area& origin);
        pipeline* create_replacement_pipeline(const libecap::StatusLine* line,
                                              const libecap::RequestLine* requestLine);

        pipeline* create_sniffer_pipeline(const libecap::StatusLine* line, const libecap::Area& trid);
    private:
        std::string origin_ip;
        csp_header* csp;
        csp_header* csp_report_only;
        sniffer_meta_data* sniffer_md;
        bool sniffer_only;
    };
} // ns binadox

#endif //BINADOX_ECAP_RESPONSE_PROCESSOR_HH
