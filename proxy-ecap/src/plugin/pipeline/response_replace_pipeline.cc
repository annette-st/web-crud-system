//
// Created by igor on 14/06/2021.
//
#include <libecap/common/errors.h>
#include <libecap/host/xaction.h>
#include <libecap/common/message.h>

#include <libecap/common/version.h>
#include "response_replace_pipeline.hh"
#include "plugin/constants.hh"
#include "backend/unused.hh"


namespace binadox
{
    response_replace_pipeline::response_replace_pipeline (std::string&& text)
    : sending_buffer(std::move(text)),
    sending_offset(0),
    last_chunk_size(0)
    {

    }
    // -------------------------------------------------------------------------------------------------------
    response_replace_pipeline::~response_replace_pipeline()
    {

    }
    // -------------------------------------------------------------------------------------------------------
    void response_replace_pipeline::consume(UNUSED_ARG const char* data, std::size_t size)
    {
        last_chunk_size = size;
    }
    // -------------------------------------------------------------------------------------------------------
    void response_replace_pipeline::finish ()
    {

    }
    // -------------------------------------------------------------------------------------------------------
    void response_replace_pipeline::update_headers (UNUSED_ARG libecap::shared_ptr<libecap::Message>& adapted)
    {

    }
    // -------------------------------------------------------------------------------------------------------
    libecap::Area response_replace_pipeline::transmit(libecap::size_type offs, UNUSED_ARG libecap::size_type size)
    {
        const std::size_t offset = sending_offset + offs;
        const std::size_t to_transmit = sending_buffer.size() - offset;
        if (offset >= sending_buffer.size())
        {
            return libecap::Area();
        }
        return libecap::Area::FromTempBuffer(sending_buffer.data() + offset, to_transmit);
    }
    // -------------------------------------------------------------------------------------------------------
    void response_replace_pipeline::update_transmitted_offset (std::size_t offs)
    {
        sending_offset = offs;
    }
    // -------------------------------------------------------------------------------------------------------
    std::size_t response_replace_pipeline::get_last_input_length() const
    {
        return last_chunk_size;
    }
    // -------------------------------------------------------------------------------------------------------
    bool response_replace_pipeline::is_finished() const
    {
        return (sending_offset >= sending_buffer.size());
    }

}