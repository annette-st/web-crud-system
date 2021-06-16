//
// Created by igor on 13/08/2020.
//

#ifndef BINADOX_ECAP_PIPELINE_HH
#define BINADOX_ECAP_PIPELINE_HH

#include <libecap/common/area.h>
#include <libecap/common/message.h>

namespace binadox
{
    class pipeline
    {
    public:
        pipeline();
        virtual ~pipeline();
    public:
        virtual void consume(const char* data, std::size_t size) = 0;
        virtual void finish () = 0;
        virtual void update_headers (libecap::shared_ptr<libecap::Message>& adapted) = 0;

        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size) = 0;
        virtual void update_transmitted_offset (std::size_t offs) = 0;
        virtual std::size_t get_last_input_length() const = 0;
        virtual bool is_finished() const = 0;
    };
} // ns binadox

#endif //BINADOX_ECAP_PIPELINE_HH
