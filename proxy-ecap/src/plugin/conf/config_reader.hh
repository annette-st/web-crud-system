//
// Created by igor on 30/07/2020.
//

#ifndef BINADOX_ECAP_CONFIG_READER_HH
#define BINADOX_ECAP_CONFIG_READER_HH

#include <string>
#include <vector>
#include "backend/fs.hh"

namespace binadox
{
    struct config
    {
        bool use_dynamic_config;
        fs::path path_to_script;
        std::string workspace_id;
        bool read_from_disk;
        fs::path work_dir;

        struct ipc
        {
            std::string type;
            std::string hostname;
            int port;
            std::string username;
            std::string password;
        };

        ipc ipc_data;

        fs::path path_to_logins;
        fs::path path_to_onload;
    };

    config read_config_file(const std::string& path);

} // ns binadox





#endif //BINADOX_ECAP_CONFIG_READER_HH
