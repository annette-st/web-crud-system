//
// Created by igor on 13/08/2020.
//

#include "plugin/pipeline/request_pipeline.hh"
#include "backend/unused.hh"

namespace binadox
{
    request_pipeline::request_pipeline()
    {

    }
    // ----------------------------------------------------------------------
    request_pipeline::~request_pipeline()
    {

    }
    // ---------------------------------------------------------------------------------------------
    void request_pipeline::update_headers (UNUSED_ARG libecap::shared_ptr<libecap::Message>& adapted)
    {

    }
    // ---------------------------------------------------------------------------------------------
    libecap::Area request_pipeline::transmit(UNUSED_ARG libecap::size_type offset, UNUSED_ARG libecap::size_type size)
    {
        return libecap::Area();
    }
    // ---------------------------------------------------------------------------------------------
    void request_pipeline::update_transmitted_offset (std::size_t /*offs*/)
    {

    }
    // ---------------------------------------------------------------------------------------------
    std::size_t request_pipeline::get_last_input_length() const
    {
        return 0;
    }
    // ---------------------------------------------------------------------------------------------
    bool request_pipeline::is_finished() const
    {
        return true;
    }
} // ns binadox
