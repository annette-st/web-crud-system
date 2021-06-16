//
// Created by igor on 14/06/2021.
//

#ifndef BINADOX_ECAP_RESPONSE_REPLACE_PIPELINE_HH
#define BINADOX_ECAP_RESPONSE_REPLACE_PIPELINE_HH

#include "plugin/pipeline/request_pipeline.hh"

namespace binadox
{
    class response_replace_pipeline : public pipeline
    {
    public:
        response_replace_pipeline (std::string&& text);
        ~response_replace_pipeline();

        virtual void consume(const char* data, std::size_t size);
        virtual void finish ();
        virtual void update_headers (libecap::shared_ptr<libecap::Message>& adapted);

        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        virtual void update_transmitted_offset (std::size_t offs);
        virtual std::size_t get_last_input_length() const;
        virtual bool is_finished() const ;
    private:
        std::string sending_buffer;
        std::size_t sending_offset;
        bool has_all_input;
        std::size_t total_input;
        std::size_t last_chunk_size;
    };
} // ns binadox


#endif //BINADOX_ECAP_RESPONSE_REPLACE_PIPELINE_HH
