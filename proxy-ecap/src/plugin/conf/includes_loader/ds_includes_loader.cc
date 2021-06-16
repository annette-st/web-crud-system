//
// Created by igor on 31/12/2020.
//

#include "ds_includes_loader.hh"
#include "plugin/string_utils.hh"

namespace binadox
{
    ds_includes_loader::ds_includes_loader(const std::map<std::string, std::string>& ds)
    : m_cache(ds)
    {
        for (const auto& kv : m_cache)
        {
            auto plain_name = kv.first;
            replace_all(plain_name, "#", "*");

            add(plain_name, kv.first);
        }
    }
    // --------------------------------------------------------------------------------------------
    std::string ds_includes_loader::load_resource(const std::string& resource) const
    {
        auto itr = m_cache.find(resource);
        if (itr != m_cache.end())
        {
            return itr->second;
        }
        return "";
    }


}
