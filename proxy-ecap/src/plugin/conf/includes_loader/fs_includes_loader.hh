//
// Created by igor on 15/09/2020.
//

#ifndef BINADOX_ECAP_FS_INCLUDES_LOADER_HH
#define BINADOX_ECAP_FS_INCLUDES_LOADER_HH

#include "plugin/conf/includes_loader/abstract_includes_loader.hh"
#include "backend/fs.hh"
namespace binadox
{
    class fs_includes_loader : public abstract_includes_loader
    {
    public:
        fs_includes_loader(const fs::path& path, bool reread);
    private:
        virtual std::string load_resource(const std::string& resource) const;
    private:
        bool m_reread;
        typedef std::map<std::string, std::string> res_map_t;
        res_map_t m_cache;
    };
}

#endif //BINADOX_ECAP_FS_INCLUDES_LOADER_HH
