//
// Created by igor on 13/07/2020.
//

#ifndef BINADOX_ECAP_REQUEST_PROCESSOR_HH
#define BINADOX_ECAP_REQUEST_PROCESSOR_HH

#include <memory>
#include "plugin/processors/basic_processor.hh"

namespace binadox
{
    class pipeline;

    class request_processor : public basic_processor
    {
    public:
        request_processor(libecap::host::Xaction *x);

        static bool wants_url(const char* url);

        virtual ~request_processor();
    private:
        virtual bool on_start_request(const libecap::RequestLine* line, const nonstd::string_view& domain);
    private:
        struct pipeline_props
        {
            pipeline_props();
            pipeline_props(std::unique_ptr<pipeline>&& handler, libecap::shared_ptr<libecap::Message> ab, bool vb);

            bool need_vb;
            libecap::shared_ptr<libecap::Message> message;
            pipeline* consumer;
        };

        typedef pipeline_props (request_processor::* factory_t)(const libecap::RequestLine* line, const nonstd::string_view& domain);
        static factory_t factory_methods[];

        pipeline_props create_ipc_pipeline(const libecap::RequestLine* line, const nonstd::string_view& domain);
        pipeline_props create_blacklist_pipeline(const libecap::RequestLine* line, const nonstd::string_view& domain);
        pipeline_props create_sniffer_pipeline(const libecap::RequestLine* line, const nonstd::string_view& domain);
    };
} // ns binadox

#endif //BINADOX_ECAP_REQUEST_PROCESSOR_HH
