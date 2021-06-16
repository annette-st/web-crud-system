//
// Created by igor on 17/08/2020.
//
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h> // stat


#include "backend//whereami.h"
#include "backend//work_dir.hh"



namespace binadox
{
    static std::string whereami_exe()
    {
        int length = wai_getExecutablePath(NULL, 0, NULL);
        if (length <= 0)
        {
            return "";
        }

        std::vector<char> out(length + 1, '\0');
        wai_getExecutablePath(out.data(), length, &length);
        return std::string(out.data());
    }
    // ----------------------------------------------------------------------------
    static std::string guess_squid_cache_folder()
    {
        std::string squid_exec = whereami_exe();
        if (squid_exec.empty())
        {
            return "";
        }
        std::string::size_type idx = squid_exec.rfind("/sbin/");
        if (idx == std::string::npos)
        {
            return "";
        }
        std::string prefix = squid_exec.substr(0, idx);
        prefix += "/var/cache/squid/";
        return prefix;
    }
    // ----------------------------------------------------------------------------
    static std::string guess_var_folder()
    {
        return "/var/cache/squid/";
    }
    // ----------------------------------------------------------------------------
    static std::string guess_local_var_folder()
    {
        return "/usr/local/var/cache/squid/";
    }
    // ----------------------------------------------------------------------------
    static std::string guess_usr_var_folder()
    {
        return "/usr/var/cache/squid/";
    }
    // ----------------------------------------------------------------------------
    typedef std::string (* guesser_t)();

    guesser_t guess_fn[] = {
            guess_squid_cache_folder,
            guess_var_folder,
            guess_local_var_folder,
            guess_usr_var_folder,
            NULL
    };
    // ----------------------------------------------------------------------------
    bool can_write(const fs::path& path)
    {
        return 0 == access(path.c_str(), W_OK);
    }
    // ----------------------------------------------------------------------------
    fs::path find_writable_location()
    {
        int k = 0;
        while (true)
        {
            guesser_t fn = guess_fn[k++];
            if (!fn)
            {
                break;
            }
            std::string r = fn();
            if (!r.empty())
            {
                if (can_write(r))
                {
                    return r;
                }
            }
        }

        return "";
    }
    // ----------------------------------------------------------------------------
    bool dir_exists(const fs::path& path)
    {
        if (fs::exists(path))
        {
            return fs::is_directory(path);
        }
        return false;
    }
    // ----------------------------------------------------------------------------
    bool mk_path(const fs::path& path)
    {
        return fs::create_directories(path);
    }
    // ---------------------------------------------------------------
    fs::path get_tmp_dir()
    {
        char *tmpdir;

        if ((tmpdir = getenv ("TEMP")) != NULL)   return tmpdir;
        if ((tmpdir = getenv ("TMP")) != NULL)    return tmpdir;
        if ((tmpdir = getenv ("TMPDIR")) != NULL) return tmpdir;

        return "/tmp";
    }
    // ---------------------------------------------------------------
    time_t get_last_modification_time(const fs::path& path)
    {
        struct stat attr;
        ::stat(path.c_str(), &attr);
        return (time_t)attr.st_mtim.tv_sec;
    }
    // ---------------------------------------------------------------
    std::string read_file_to_string(const fs::path& path)
    {
        std::ifstream stream(path, std::ios::in | std::ios::binary);
        stream.exceptions(std::ios_base::badbit);
        return std::string(std::istreambuf_iterator<char>{stream}, {});
    }
} // ns binadox
