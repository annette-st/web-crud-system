//
// Created by igor on 19/07/2020.
//

#include "plugin/filter/glob_match.hh"
// https://github.com/MichaelCook/glob_match
#define GLOB_MATCH_EQUAL_CHARS(A, B) ((A) == (B))
namespace
{
    bool glob_match_internal(char const* p, char const* pe,
                             char const* q, char const* qe)
    {
        char const* backtracker;
        for (;;)
        {
            if (p == pe)
                return q == qe;
            if (*p == '*')
            {
                ++p;
                for (backtracker = qe; backtracker >= q; --backtracker)
                    if (glob_match_internal(p, pe, backtracker, qe))
                        return true;
                break;
            }
            if (q == qe)
                break;
            if (*p != '?' && !GLOB_MATCH_EQUAL_CHARS(*p, *q))
                break;
            ++p, ++q;
        }
        return false;
    }
}

namespace binadox
{
    bool glob_match(const std::string& pattern, const char* text, std::size_t length)
    {
        return glob_match_internal(pattern.c_str(), pattern.c_str() + pattern.size(),
                                   text, text + length);
    }
    // -------------------------------------------------------------------------------------------------
    bool glob_match(const std::string& pattern, const nonstd::string_view & text)
    {

        return glob_match_internal(pattern.c_str(), pattern.c_str() + pattern.size(),
                                   text.data(), text.data() + text.size());
    }
    // -------------------------------------------------------------------------------------------------
    bool glob_match(const std::string& pattern, const std::string& text)
    {
        return glob_match(pattern, text.c_str(), text.size());
    }
} // ns binadox

// ====================================================================================
// UNITTEST
// ====================================================================================

#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

#define doGLOB(A, B) binadox::glob_match(A, nonstd::string_view(B))

TEST_CASE("GLOB")
{
    REQUIRE(doGLOB("hello", "hello"));
    REQUIRE(!doGLOB("hello", "hello!"));
    REQUIRE(!doGLOB("hello", "hi"));

    REQUIRE(doGLOB("he?lo", "hello"));
    REQUIRE(doGLOB("h*o", "hello"));
    REQUIRE(doGLOB("h******o", "hello"));
    REQUIRE(doGLOB("h***?***o", "hello"));
    REQUIRE(doGLOB("*o", "hello"));
    REQUIRE(doGLOB("h*", "hello"));

    REQUIRE(!doGLOB("", "hello"));
    REQUIRE(doGLOB("", ""));
    REQUIRE(doGLOB("*", ""));
    REQUIRE(doGLOB("*", "hello"));
    REQUIRE(!doGLOB("?", ""));

    REQUIRE(doGLOB("h***?***o", "hello"));

    REQUIRE(!doGLOB("hello", "HELLO"));
    REQUIRE(!doGLOB("h*L?", "hello"));

}

#endif
