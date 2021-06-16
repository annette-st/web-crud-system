//
// Created by igor on 17/08/2020.
//

#ifndef BINADOX_ECAP_WORK_DIR_HH
#define BINADOX_ECAP_WORK_DIR_HH

#include <string>
#include <time.h>
#include "backend/fs.hh"

namespace binadox
{
    fs::path find_writable_location();
    bool mk_path(const fs::path& path);
    bool dir_exists(const fs::path& path);
    bool can_write(const fs::path& path);
    fs::path get_tmp_dir();
    time_t get_last_modification_time(const fs::path& path);

    std::string read_file_to_string(const fs::path& path);
} // ns binadox

#endif //BINADOX_ECAP_WORK_DIR_HH
