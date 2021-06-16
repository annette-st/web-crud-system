//
// Created by igor on 15/09/2020.
//

#include <fstream>
#include <sstream>

#include "plugin/logger.hh"
#include "plugin/conf/includes_loader/fs_includes_loader.hh"
#include "plugin/string_utils.hh"
#include "backend/fs.hh"

static std::string read_file(const fs::path& path)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

namespace binadox
{
    fs_includes_loader::fs_includes_loader(const fs::path& path, bool reread)
    : m_reread(reread)
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            LOG_TRACE("Can not load includes from", path);
            return;
        }
        LOG_TRACE("Reading includes from", path);
        for (const auto& fsentry : fs::directory_iterator(path))
        {
            if (fsentry.is_directory())
            {
                continue;
            }
            auto name = fsentry.path().filename();
            const auto ext = name.extension();
            if (ext == ".js")
            {
                auto full_path = path / name;
                if (!reread)
                {
                    m_cache[full_path] = read_file(full_path);
                }
                auto plain_name = name.replace_extension().string();
                replace_all(plain_name, "#", "*");

                add(plain_name, full_path);
                LOG_TRACE("Loading include ", path.filename(), plain_name);
            }
        }
    }
    // ------------------------------------------------------------------------------------------------
    std::string fs_includes_loader::load_resource(const std::string& resource) const
    {
        res_map_t::const_iterator i = m_cache.find(resource);
        if (i != m_cache.end())
        {
            return i->second;
        }
        return read_file(resource);
    }
    // ------------------------------------------------------------------------------------------------
} // ns binadox

// =================================================================================================
// UNITTEST
// =================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

TEST_CASE ("fs_includes_loader")
{
//   binadox::fs_includes_loader("/home/igor/proj/binadox/binadox-ecap/scripts/login_config/", false);
}
#endif