//
// Created by igor on 19/07/2020.
//

#ifndef BINADOX_ECAP_GLOB_MATCH_HH
#define BINADOX_ECAP_GLOB_MATCH_HH

#include <string>
#include <nonstd/string_view.hpp>
namespace binadox
{
    bool glob_match(const std::string& pattern, const char* text, std::size_t length);
    bool glob_match(const std::string& pattern, const nonstd::string_view & text);
    bool glob_match(const std::string& pattern, const std::string& text);
}


#endif //BINADOX_ECAP_GLOB_MATCH_HH
