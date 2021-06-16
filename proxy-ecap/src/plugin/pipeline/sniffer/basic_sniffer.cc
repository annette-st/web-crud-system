//
// Created by igor on 23/08/2020.
//

#include <string.h>
#include <libecap/host/xaction.h>
#include "plugin/pipeline/sniffer/basic_sniffer.hh"

#include "plugin/conf/configurator.hh"
#include "plugin/sniffer_db/sniffer_db.hh"

#include "backend/unused.hh"
#include "plugin/logger.hh"

namespace binadox
{
    basic_sniffer::basic_sniffer ()
    : meta_data(nullptr),
      chunk_number(0),
      sending_offset(0),
      all_finished(false)
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    basic_sniffer::~basic_sniffer()
    {
        delete meta_data;
    }
    // -----------------------------------------------------------------------------------------------------------
    void basic_sniffer::use_sniffer_metadata(sniffer_meta_data* md)
    {
        if (md->is_request())
        {
            auto url = md->get_request().get_url();
            LOG_TRACE("START REQUEST transaction ", md->get_transaction_id(), " URL: ", url);
        }
        else
        {
            LOG_TRACE("START RESPONSE transaction ", md->get_transaction_id());
        }
        meta_data = md;
        get_config()->sniffer_database().put(*meta_data);
    }
    // -----------------------------------------------------------------------------------------------------------
    sniffer_meta_data* basic_sniffer::get_meta_data ()
    {
        return meta_data;
    }
    // -----------------------------------------------------------------------------------------------------------
    void basic_sniffer::consume(const char* data, std::size_t size)
    {
        sending_offset = 0;
        body_chunk.resize(size);
        memcpy(body_chunk.data(), data, size);
        get_config()->sniffer_database().put(*meta_data, chunk_number, body_chunk);
        chunk_number++;
    }
    // -----------------------------------------------------------------------------------------------------------
    void basic_sniffer::finish ()
    {
        if (meta_data)
        {

            if (meta_data->is_request())
            {
                auto url = meta_data->get_request().get_url();
                LOG_TRACE("FINISH REQUEST transaction ", meta_data->get_transaction_id(), " URL: ", url);
            }
            else
            {
                LOG_TRACE("FINISH RESPONSE transaction ", meta_data->get_transaction_id());
            }
            get_config()->sniffer_database().transaction_finished(*meta_data);
        }
        all_finished = true;
    }
    // -----------------------------------------------------------------------------------------------------------
    bool basic_sniffer::is_finished() const
    {
        return all_finished;
    }
    // -----------------------------------------------------------------------------------------------------------
    libecap::Area basic_sniffer::transmit(libecap::size_type offs, UNUSED_ARG libecap::size_type size)
    {
        const std::size_t offset = sending_offset + offs;
        const std::size_t to_transmit = body_chunk.size() - offset;
        if (offset >= body_chunk.size())
        {
            return libecap::Area();
        }
        return libecap::Area::FromTempBuffer(body_chunk.data() + offset, to_transmit);
    }
    // -----------------------------------------------------------------------------------------------------------
    void basic_sniffer::update_transmitted_offset (std::size_t offs)
    {
        sending_offset += offs;
    }
    // -----------------------------------------------------------------------------------------------------------
    std::size_t basic_sniffer::get_last_input_length() const
    {
        return body_chunk.size();
    }
    // ------------------------------------------------------------------------------------------------------------
    void basic_sniffer::update_headers (UNUSED_ARG libecap::shared_ptr<libecap::Message>& adapted)
    {

    }
} // ns binadox
