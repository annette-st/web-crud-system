//
// Created by igor on 15/08/2020.
//

#ifndef BINADOX_ECAP_BLACKLIST_PIPELINE_HH
#define BINADOX_ECAP_BLACKLIST_PIPELINE_HH

#include "plugin/pipeline/request_pipeline.hh"

namespace binadox
{
    class blacklist_pipeline : public request_pipeline
    {
    public:
        blacklist_pipeline ();
        ~blacklist_pipeline();

        static bool accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        static bool wants_url(const char* url, std::size_t url_len);
    private:
        virtual bool init(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        virtual libecap::shared_ptr<libecap::Message> generate_response();


        virtual void consume(const char* data, std::size_t size);
        virtual void finish ();

        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        virtual void update_transmitted_offset (std::size_t offs) ;
        virtual std::size_t get_last_input_length() const;
    private:
        libecap::shared_ptr<libecap::Message> adapted;
        libecap::Area error_page;
        bool send;
    };
} // ns binadox



#endif //BINADOX_ECAP_BLACKLIST_PIPELINE_HH
