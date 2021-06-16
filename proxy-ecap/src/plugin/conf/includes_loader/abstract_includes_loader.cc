//
// Created by igor on 15/09/2020.
//

#include "plugin/conf/includes_loader/abstract_includes_loader.hh"
#include "plugin/filter/glob_match.hh"

namespace binadox
{
    abstract_includes_loader::~abstract_includes_loader()
    {

    }
    // --------------------------------------------------------------------------------------------------------
    std::string abstract_includes_loader::get_by_domain(const std::string& domain) const
    {
        for (glob_to_resource_map_t::const_iterator i = m_globs.begin(); i != m_globs.end(); i++)
        {
            if (glob_match(i->first, domain))
            {
                return load_resource(i->second);
            }
        }
        return "";
    }
    // --------------------------------------------------------------------------------------------------------
    std::string abstract_includes_loader::get_by_domain(const nonstd::string_view& domain) const
    {
        for (glob_to_resource_map_t::const_iterator i = m_globs.begin(); i != m_globs.end(); i++)
        {
            if (glob_match(i->first, domain))
            {
                return load_resource(i->second);
            }
        }
        return "";
    }
    // --------------------------------------------------------------------------------------------------------
    void abstract_includes_loader::add(const std::string& glob_pattern, const std::string& resource)
    {
        m_globs[glob_pattern] = resource;
    }
}