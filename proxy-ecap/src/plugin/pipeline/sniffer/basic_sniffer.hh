//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_BASIC_SNIFFER_HH
#define BINADOX_ECAP_BASIC_SNIFFER_HH

#include <vector>

#include "plugin/pipeline/request_pipeline.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"

namespace binadox
{
    class basic_sniffer : public pipeline
    {
    public:
        basic_sniffer ();
        virtual ~basic_sniffer();

        virtual void finish ();

        virtual libecap::shared_ptr<libecap::Message> generate_response() = 0;
        sniffer_meta_data* get_meta_data ();
    private:

        virtual void consume(const char* data, std::size_t size);


        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        virtual void update_transmitted_offset (std::size_t offs) ;
        virtual std::size_t get_last_input_length() const;

        virtual bool is_finished() const;

        virtual void update_headers (libecap::shared_ptr<libecap::Message>& adapted);
    protected:
        void use_sniffer_metadata(sniffer_meta_data* md);
    private:
        sniffer_meta_data* meta_data;
        std::vector<char> body_chunk;
        int chunk_number;
        std::size_t sending_offset;
        bool all_finished;
    };
} // ns binadox

#endif //BINADOX_ECAP_SNIFFER_REQ_PIPELINE_HH
