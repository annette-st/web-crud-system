//
// Created by igor on 13/08/2020.
//

#ifndef BINADOX_ECAP_REQUEST_PIPELINE_HH
#define BINADOX_ECAP_REQUEST_PIPELINE_HH

#include <libecap/common/forward.h>
#include <libecap/common/header.h>

#include "plugin/pipeline/pipeline.hh"
#include "plugin/conf/configurator.hh"

namespace binadox
{
    class request_pipeline : public pipeline
    {
    public:
        request_pipeline();
        virtual ~request_pipeline();


        virtual bool init(libecap::host::Xaction *hostx, const libecap::RequestLine* line) = 0;
        virtual libecap::shared_ptr<libecap::Message> generate_response() = 0;

        virtual void update_headers (libecap::shared_ptr<libecap::Message>& adapted);
        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        virtual void update_transmitted_offset (std::size_t offs) ;
        virtual std::size_t get_last_input_length() const;
        virtual bool is_finished() const;
    };
} // ns binadox

#endif //BINADOX_ECAP_REQUEST_PIPELINE_HH
