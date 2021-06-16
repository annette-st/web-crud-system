//
// Created by igor on 23/08/2020.
//

#include "plugin/filter/sniffer.hh"
#include "plugin/filter/tld.hh"

namespace binadox
{
    sniffer::sniffer(bool all)
    : accept_all(all)
    {

    }
    // -----------------------------------------------------------------------------------------
    void sniffer::add(const std::string& domain)
    {
        if (!accept_all)
        {
            domains.insert(domain);
        }
    }
    // -----------------------------------------------------------------------------------------
    bool sniffer::accept_url(const nonstd::string_view& domain) const
    {
        if (accept_all)
        {
            return true;
        }
        return domains.contains(get_tld_from_url(domain));
    }
    // -----------------------------------------------------------------------------------------
    bool sniffer::accept_domain(const nonstd::string_view& domain) const
    {
        if (accept_all)
        {
            return true;
        }
        return domains.contains(get_tld_from_domain(domain));
    }
} // ns binadox
