//
// Created by igor on 01/09/2020.
//

#ifndef BINADOX_ECAP_SCRIPT_DATA_HH
#define BINADOX_ECAP_SCRIPT_DATA_HH

#include <string>
#include <time.h>
#include "plugin/template_engine/te.hh"
#include <nonstd/string_view.hpp>

namespace binadox
{
    class script_data
    {
    public:
        script_data(const std::string& text, const time_t& timestamp);
        te& get_engine();
        time_t get_timestamp() const;
    private:
        te     text;
        time_t timestamp;

    };
} // ns binadox

#endif //BINADOX_ECAP_SCRIPT_DATA_HH
