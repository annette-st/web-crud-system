//
// Created by igor on 23/08/2020.
//

#include <algorithm>
#include "plugin/string_utils.hh"

namespace binadox
{
    bool iequal(char ch1, char ch2)
    {
        char t1 = ((ch1 >= 'a') && (ch1 <= 'z')) ? ch1 - ('a' - 'A') : ch1;
        char t2 = ((ch2 >= 'a') && (ch1 <= 'z')) ? ch2 - ('a' - 'A') : ch2;
        return t1 == t2;
    }
    // ---------------------------------------------------------------------------------------------------------
    bool iequals(const std::string& a, const std::string& b)
    {
        unsigned int sz = a.size();
        if (b.size() != sz)
        {
            return false;
        }
        for (unsigned int i = 0; i < sz; ++i)
        {
            if (!iequal(a[i], b[i]))
            {
                return false;
            }
        }
        return true;
    }
    // ---------------------------------------------------------------------------------------------------------
    bool iequals(const libecap::Name& a, const libecap::Name& b)
    {
        return iequals(a.image(), b.image());
    }
    // ---------------------------------------------------------------------------------------------------------
    bool icase_contains(const libecap::Area& area, const std::string& what)
    {
        if (area.size < what.size())
        {
            return false;
        }
        const char* area_begin = area.start;
        const char* area_end = area_begin + area.size;
        const char* what_begin = what.c_str();
        const char* what_end = what_begin + what.size();

        const char* pos = std::search(
                area_begin, area_end,
                what_begin, what_end,
                iequal);
        return (pos != area_end);
    }
    // ------------------------------------------------------------------------------------------------------
    bool ends_with(const std::string& str, const std::string& suffix)
    {
        return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
    }
    // ------------------------------------------------------------------------------------------------------
    bool starts_with(const std::string& str, const std::string& prefix)
    {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
    }
    // ------------------------------------------------------------------------------------------------------
    void replace_all(std::string& str, const std::string& from, const std::string& to)
    {
        if(from.empty())
        {
            return;
        }
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
} // ns binadox
