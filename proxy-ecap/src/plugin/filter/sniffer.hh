//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_HH
#define BINADOX_ECAP_SNIFFER_HH

#include "plugin/filter/trie.hh"
#include <string>
#include <nonstd/string_view.hpp>

namespace binadox
{
    class sniffer
    {
    public:
        sniffer(bool all);
        void add(const std::string& domain);

        bool accept_url(const nonstd::string_view& domain) const;
        bool accept_domain(const nonstd::string_view& domain) const;
    private:
        bool accept_all;
        trie domains;
    };
}

#endif //BINADOX_ECAP_SNIFFER_HH
