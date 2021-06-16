//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_STRING_UTILS_HH
#define BINADOX_ECAP_STRING_UTILS_HH

#include <libecap/common/name.h>
#include <libecap/common/area.h>

#include <string>
#include <vector>

namespace binadox
{
    // case insensitive functions
    bool iequal(char ch1, char ch2);
    bool iequals(const std::string& a, const std::string& b);
    bool iequals(const libecap::Name& a, const libecap::Name& b);
    bool icase_contains(const libecap::Area& area, const std::string& what);

    template <typename T>
    void tokenize(const std::string& input, std::vector<T>& toks, char delim)
    {
        size_t start;
        size_t end = 0;
        while ((start = input.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = input.find(delim, start);
            toks.push_back(T(input.substr(start, end - start)));
        }
    }

    bool ends_with(const std::string& str, const std::string& suffix);
    bool starts_with(const std::string& str, const std::string& prefix);
    void replace_all(std::string& str, const std::string& from, const std::string& to);
}


#endif //BINADOX_ECAP_STRING_UTILS_HH
