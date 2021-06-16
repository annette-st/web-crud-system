//
// Created by igor on 17/09/2020.
//

#ifndef BINADOX_ECAP_FS_HH
#define BINADOX_ECAP_FS_HH
#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
#endif //BINADOX_ECAP_FS_HH
