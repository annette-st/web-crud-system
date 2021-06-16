//
// Created by igor on 15/11/2020.
//

#include <exception>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

#include "archive.hh"
#include "zip/ZipArchive.hpp"
#include "zip/ZipException.hpp"
#include "backend/fs.hh"


namespace
{
    struct top_level
    {
        uint64_t timestamp;
        std::string name;

        top_level()
        : timestamp(0),
        name("")
        {}

        static top_level load(const std::string& root, const std::string& name)
        {
            top_level result;
            fs::path full_path = fs::path(root) / name / "REQ" / "headers";
            if (fs::exists(full_path) && fs::is_regular_file(full_path))
            {
                std::ifstream ifs(full_path);
                auto jf = nlohmann::json::parse(ifs);
                if (jf.contains("time"))
                {
                    result.name = name;
                    result.timestamp = jf["time"].get<uint64_t>();
                }
            }
            return result;
        }
    };
    bool operator < (const top_level& a, const top_level& b)
    {
        return a.timestamp < b.timestamp;
    }
    // ===============================================================================
    void append_to_archive(Zippy::ZipArchive& archive, const fs::path& root, const std::string& root_base_name)
    {

        for (const auto& fsentry : fs::directory_iterator(root))
        {
            if (fsentry.is_regular_file())
            {
                auto full_path = fsentry.path().string();
                auto idx = full_path.find(root_base_name);
                if (idx != std::string::npos)
                {
                    auto zip_name = full_path.substr(idx);
                    std::ifstream stream(full_path, std::ios::in | std::ios::binary);
                    std::vector<unsigned char> contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
                    archive.AddEntry(zip_name, contents);
                }
            }
            else
            {
                append_to_archive(archive, fsentry.path(), root_base_name);
            }
        }
    }
}

namespace binadox
{
    namespace sniffer
    {
        void create_output_archive(const std::string& root)
        {
            fs::path path(root);
            if (!fs::exists(path) || !fs::is_directory(path))
            {
                throw std::runtime_error("Can not load data from " + root);
            }
            Zippy::ZipArchive arch;
            const std::string base_name = path.filename().string() + ".zip";
            arch.Create(base_name);

            std::vector<top_level> meta;

            for (const auto& fsentry : fs::directory_iterator(path))
            {
                if (!fsentry.is_directory())
                {
                    continue;
                }
                auto name = fsentry.path().filename();
                top_level ti = top_level::load(root, name);
                if (ti.timestamp > 0)
                {
                    meta.push_back(ti);
                    std::string root_base_name = fsentry.path().filename();
                    append_to_archive(arch, fsentry.path(), root_base_name);
                }
            }
            std::sort(meta.begin(), meta.end());

            std::ostringstream os;
            for (const auto& tl : meta)
            {
                os << tl.name << "\n";
            }

            arch.AddEntry("index.txt", os.str());

            arch.Save();
            arch.Close();
        }
    }
}