//
// Created by igor on 17/02/2021.
//

#ifndef BINADOX_ECAP_CRC_64_HH
#define BINADOX_ECAP_CRC_64_HH

#include <cstdint>
#include <string>
#include <nonstd/string_view.hpp>

namespace binadox
{
    uint64_t crc64(uint64_t crc, const unsigned char *s, std::size_t length);
    uint64_t crc64(uint64_t crc, const std::string& s);
    uint64_t crc64(uint64_t crc, const nonstd::string_view& s);

    uint64_t crc64(const unsigned char *s, std::size_t length);
    uint64_t crc64(const std::string& s);
    uint64_t crc64(const nonstd::string_view& s);
}


#endif //BINADOX_ECAP_CRC_64_HH
