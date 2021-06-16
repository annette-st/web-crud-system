//
// Created by igor on 16/08/2020.
//

#include <string.h>
#include <algorithm>
#include "plugin/filter/tld.hh"
#include "plugin/filter/regdom/registry_controlled_domain.h"

template <typename STRING>
STRING get_domain_IMPL(const STRING& url, bool with_schema)
{
    static const char* const doubleQ = "//";
    static const std::size_t n = strlen(doubleQ);
    static const char* const doubleQ_end = doubleQ + n;
    const char* url_end = url.data() + url.size();
    const char* last = std::search(url.data(), url_end, doubleQ, doubleQ_end);
    const char* start_domain = nullptr;

    if (last == url_end || static_cast<uintptr_t>((url_end - last)) < n)
    {
        start_domain = url.data();
    } else
    {
        start_domain = last + n;
    }


    static const char* const singleQ = "/";
    static const char* const port = ":";
    const char* end_domain = std::search(start_domain, url_end, singleQ, singleQ + 1);
    end_domain = std::search(start_domain, end_domain, port, port + 1);

    if (with_schema)
    {
        start_domain = url.data();
    }

    const std::size_t domain_len = end_domain - start_domain;
    return STRING(start_domain, domain_len);
}

namespace binadox
{
    std::string get_domain(const std::string& url, bool with_schema)
    {
        return get_domain_IMPL(url, with_schema);
    }
    // -------------------------------------------------------------------------------------------------
    nonstd::string_view get_domain(const nonstd::string_view& url, bool with_schema)
    {
        return get_domain_IMPL(url, with_schema);
    }
    // -------------------------------------------------------------------------------------------
    std::string get_tld_from_url(const nonstd::string_view& url)
    {
        return get_tld_from_domain(get_domain(url, false));
    }
    // -------------------------------------------------------------------------------------------
    std::string get_tld_from_url(const nonstd::string_view& url, nonstd::string_view& domain)
    {
          domain = get_domain(url, false);
          return get_tld_from_domain(domain);
    }
    // -------------------------------------------------------------------------------------------
    std::string get_tld_from_domain(const nonstd::string_view& domain)
    {
        using namespace chromium::registry_controlled_domains;
        if (domain.empty())
        {
            return std::string();
        }
        return GetDomainAndRegistry(domain, INCLUDE_PRIVATE_REGISTRIES);
    }
} // ns binadox
// =================================================================================================
// UNITTEST
// =================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

static std::string get_domain(const char* url, bool with_scheme)
{
    return binadox::get_domain(std::string(url), with_scheme);
}

TEST_SUITE ("regdom")
{
    //"registered.com sub.registered.com parliament.uk sub.registered.valid.uk registered.somedom.kyoto.jp invalid-fqdn org academy.museum sub.academy.museum subsub.sub.academy.museum sub.nic.pa registered.sb sub.registered.sb abc.xyz.co.uk abc.co.uk abc.nhs.uk abc.xyz.zz example.uk d.example.uk subsub.registered.something.zw subsub.registered.9.bg registered.co.bi sub.registered.bi subsub.registered.ee ua"
    TEST_CASE("get_domain")
    {
        REQUIRE(get_domain("zopa.com", false) == "zopa.com");
        REQUIRE(get_domain("zopa.com/", false) == "zopa.com");
        REQUIRE(get_domain("zopa.com/1", false) == "zopa.com");
        REQUIRE(get_domain("zopa.com/1/", false) == "zopa.com");
        REQUIRE(get_domain("zopa.com/1/2/3/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com/1", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com/1/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com/1/2/3/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1/", false) == "zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1/2/3/", false) == "zopa.com");


        REQUIRE(get_domain("http://zopa.com", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com/", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com/1", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com/1/", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com/1/2/3/", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1/", true) == "http://zopa.com");
        REQUIRE(get_domain("http://zopa.com:8080/1/2/3/", true) == "http://zopa.com");
    }
    TEST_CASE("registered_domain")
    {

        struct {
            std::string url;
            std::string expected_domain_and_registry;
        } kTestCases[] = {
                {"http://a.baz.jp/file.html", "baz.jp"},
          {"http://a.baz.jp./file.html", "baz.jp."},
          {"http://ac.jp", ""},
          {"http://a.bar.jp", "bar.jp"},
          {"http://bar.jp", "bar.jp"},
          {"http://baz.bar.jp", "bar.jp"},
          {"http://a.b.baz.bar.jp", "bar.jp"},

          {"http://baz.pref.bar.jp", "bar.jp"},
          {"http://a.b.bar.baz.com.", "baz.com."},

          {"http://a.d.c", "d.c"},
          {"http://.a.d.c", "d.c"},
          {"http://..a.d.c", "d.c"},
          {"http://a.b.c", "b.c"},
          {"http://baz.com", "baz.com"},
          {"http://baz.com.", "baz.com."},

          {"", ""},
          {"http://", ""},
          {"file:///C:/file.html", ""},
          {"http://foo.com..", ""},
          {"http://...", ""},
          {"http://[2001:0db8:85a3:0000:0000:8a2e:0370:7334]/", ""},
          {"http://localhost", ""},
          {"http://localhost.", ""},
          {"http:////Comment", ""}
        };
        for (const auto& test_case : kTestCases)
        {
            REQUIRE(binadox::get_tld_from_url(test_case.url) == test_case.expected_domain_and_registry);
        }
    }
}
#endif