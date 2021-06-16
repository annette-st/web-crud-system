//
// Created by igor on 15/09/2020.
//

#ifndef BINADOX_ECAP_ABSTRACT_INCLUDES_LOADER_HH
#define BINADOX_ECAP_ABSTRACT_INCLUDES_LOADER_HH

#include <map>
#include <string>
#include <nonstd/string_view.hpp>

namespace binadox
{
    class abstract_includes_loader
    {
    public:
        virtual ~abstract_includes_loader();

        std::string get_by_domain(const std::string& domain) const;
        std::string get_by_domain(const nonstd::string_view& domain) const;
    protected:
        void add(const std::string& glob_pattern, const std::string& resource);
        virtual std::string load_resource(const std::string& resource) const = 0;
    private:
        typedef std::map<std::string, std::string> glob_to_resource_map_t;

        glob_to_resource_map_t m_globs;
    };
} // ns binadox

#endif //BINADOX_ECAP_ABSTRACT_INCLUDES_LOADER_HH
