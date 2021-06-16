//
// Created by igor on 31/12/2020.
//

#ifndef BINADOX_ECAP_DS_INCLUDES_LOADER_HH
#define BINADOX_ECAP_DS_INCLUDES_LOADER_HH

#include <map>
#include "plugin/conf/includes_loader/abstract_includes_loader.hh"

namespace binadox
{
    class ds_includes_loader : public abstract_includes_loader
    {
    public:
        ds_includes_loader(const std::map<std::string, std::string>& ds);
    private:
        std::string load_resource(const std::string& resource) const override;
    private:
        std::map<std::string, std::string> m_cache;
    };
}

#endif //BINADOX_ECAP_DS_INCLUDES_LOADER_HH
