//
// Created by igor on 01/09/2020.
//

#include "plugin/pipeline/script/script_data.hh"

namespace binadox
{
    // ------------------------------------------------------------------------------------
    script_data::script_data(const std::string& atext, const time_t& atimestamp)
    : text(atext),
    timestamp(atimestamp)
    {

    }
    // ------------------------------------------------------------------------------------
    te& script_data::get_engine()
    {
        return text;
    }
    // ------------------------------------------------------------------------------------
    time_t script_data::get_timestamp() const
    {
        return timestamp;
    }
} // ns binadox