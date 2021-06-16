//
// Created by igor on 30/07/2020.
//

#include "plugin/conf/config_reader.hh"
#include "plugin/conf/ini_reader.hh"
#include "plugin/string_utils.hh"
#include "plugin/logger.hh"
#include "backend/fs.hh"

namespace binadox
{
    config read_config_file(const std::string& path)
    {
        config cfg;

        INIReader reader(path);
        if (reader.ParseError() != 0)
        {
            RAISE_EX("Failed to parse ini file", path);
        }

        if (!reader.HasSection("binadox"))
        {
            RAISE_EX("Can not find [binadox] section in ini file",  path);
        }

        cfg.use_dynamic_config = reader.GetInteger("binadox", "use_dynamic_config", 0) == 1;
        cfg.path_to_script = reader.GetString("binadox", "path_to_script", "");
        cfg.workspace_id = reader.GetString("binadox", "workspace", "");
        cfg.read_from_disk = reader.GetInteger("binadox", "always_read", 0) == 1;
        cfg.work_dir = reader.GetString("binadox", "workdir", "");
        cfg.path_to_logins = reader.GetString("binadox", "path_to_logins", "");
        if (cfg.path_to_logins.empty())
        {
            cfg.path_to_logins = cfg.path_to_script.parent_path() / "login_config";
        }

        cfg.path_to_onload = reader.GetString("binadox", "path_to_onload", "");
        if (cfg.path_to_onload.empty())
        {
            cfg.path_to_onload = cfg.path_to_script.parent_path() / "onload_config";
        }

        if (!reader.HasSection("ipc"))
        {
            cfg.ipc_data.type = "redis";
            cfg.ipc_data.hostname = "127.0.0.1";
            cfg.ipc_data.port = 6379;
            cfg.ipc_data.username = "";
            cfg.ipc_data.password = "";
        } else
        {
            cfg.ipc_data.type = reader.GetString("ipc", "type", "redis");
            cfg.ipc_data.hostname = reader.GetString("ipc", "hostname", "");
            cfg.ipc_data.port = reader.GetInteger("ipc", "port", 6379);
            cfg.ipc_data.username = reader.GetString("ipc", "username", "");
            cfg.ipc_data.password = reader.GetString("ipc", "password", "");
        }

        return cfg;
    }
} // ns binadox