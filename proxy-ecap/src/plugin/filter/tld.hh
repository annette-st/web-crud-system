//
// Created by igor on 16/08/2020.
//

#ifndef BINADOX_ECAP_TLD_HH
#define BINADOX_ECAP_TLD_HH

#include <string>
#include <nonstd/string_view.hpp>

namespace binadox
{
    std::string get_domain(const std::string& url, bool with_schema);
    nonstd::string_view get_domain(const nonstd::string_view& url, bool with_schema);
    std::string get_tld_from_url(const nonstd::string_view& url);
    std::string get_tld_from_url(const nonstd::string_view& url, nonstd::string_view& domain);
    std::string get_tld_from_domain(const nonstd::string_view& domain);
} // ns tld

#endif //BINADOX_ECAP_TLD_HH
