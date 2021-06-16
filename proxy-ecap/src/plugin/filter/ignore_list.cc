//
// Created by igor on 19/07/2020.
//

#include <algorithm>
#include <string.h>
#include "plugin/filter/ignore_list.hh"
#include "plugin/filter/tld.hh"

namespace binadox
{
    ignore_list::ignore_list()
    : data_source_observer(data_source::eIGNORE_LIST)
    {
    }
    // -----------------------------------------------------------------
    ignore_list::~ignore_list()
    {

    }
    // -----------------------------------------------------------------
    void ignore_list::handle_data_source(data_source::type_t type, const data_source* ds)
    {
        if (type == data_source::eIGNORE_LIST)
        {
            filter.clear();
            for (const auto& pattern : ds->get_ignore_list())
            {
                filter.insert(pattern);
            }
        }
    }
    // -----------------------------------------------------------------
    bool ignore_list::accept(const libecap::Area &url, nonstd::string_view& domain) const
    {

        if (url.size < 5)
        {
            return true; // invalid url
        }
        nonstd::string_view found_domain = get_domain(nonstd::string_view(url.start, url.size), true);
        auto pos = found_domain.find("://");
        nonstd::string_view test_domain = found_domain;
        if (pos != nonstd::string_view::npos)
        {
            test_domain.remove_prefix(pos + 3);
        }
        domain = found_domain;
        if (filter.contains(test_domain))
        {
            return true;
        }
        return false;
    }
} // ns binadox
