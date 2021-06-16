//
// Created by igor on 16/08/2020.
//
#include <iostream>
#include <memory>
#include "plugin/filter/blacklist.hh"
#include "plugin/filter/trie.hh"
#include "plugin/filter/tld.hh"
#include <nonstd/string_view.hpp>
#include "plugin/logger.hh"

namespace binadox
{
    struct blacklist_impl
    {
        blacklist_impl()
        : domains(new trie)
        {

        }
        std::unique_ptr<trie> domains;
    };
    // ----------------------------------------------------------------------------------
    blacklist::blacklist()
    : data_source_observer(data_source::eBLACK_LIST)
    {
        pimpl = new blacklist_impl;
    }
    // ----------------------------------------------------------------------------------
    blacklist::~blacklist()
    {
        delete pimpl;
    }
    // ----------------------------------------------------------------------------------
    bool blacklist::empty() const
    {
        return pimpl->domains->empty();
    }
    // ----------------------------------------------------------------------------------
    void blacklist::handle_data_source(data_source::type_t type, const data_source* ds)
    {
        if (type == data_source::eBLACK_LIST)
        {
            for (const auto& url : ds->get_black_list())
            {
                insert(url);
            }
        }
    }
    // ----------------------------------------------------------------------------------
    bool blacklist::accept(const char* url, size_t len) const
    {
        auto sv = nonstd::string_view(url, len);
        auto d = get_tld_from_url(sv);
        auto rc = pimpl->domains->contains(d);
        if (rc)
        {
            LOG_ERROR("Blocked", sv ,"->", d);
        }

        return rc;
    }
    // ----------------------------------------------------------------------------------
    void blacklist::clear ()
    {
        pimpl->domains.reset(new trie);
    }
    // ----------------------------------------------------------------------------------
    void blacklist::insert(const std::string& pattern)
    {
        pimpl->domains->insert(pattern);
    }
} // ns binadox
