//
// Created by igor on 14/06/2021.
//

#include <utility>
#include <vector>
#include <set>
#include <mutex>
#include <regex>
#include <algorithm>

#include "plugin/conf/key_value_storage.hh"
#include "plugin/constants.hh"
#include "replace_engine.hh"

static std::regex create_re(const std::string& v)
{
    return std::regex(v, std::regex::icase | std::regex::optimize | std::regex::ECMAScript);
}

namespace binadox {

    struct replace_engine::impl
    {
        explicit impl(key_value_storage* kvs)
        : m_kvs(kvs) {}

        key_value_storage* m_kvs;

        std::vector<std::pair<std::string, std::regex>> m_regex;
        std::set<std::string> m_exact;

        mutable std::mutex m_mtx;
    };

    struct exact_handler : public key_handler
    {
        exact_handler(std::set<std::string>& exact) : m_exact(exact){

        }
        void handle(const std::string& key) override {
            m_exact.insert(key);
        }
        std::set<std::string>& m_exact;
    };

    struct re_handler : public key_handler
    {
        re_handler(std::vector<std::pair<std::string, std::regex>>& regex) : m_regex(regex){

        }
        void handle(const std::string& key) override {
            try {
                m_regex.emplace_back(key, create_re(key));
            } catch (...) {
                m_failed.push_back(key);
            }
        }
        std::vector<std::pair<std::string, std::regex>>& m_regex;
        std::vector<std::string> m_failed;
    };
    using lock_t = std::lock_guard<std::mutex>;
    // ---------------------------------------------------------------------------------------
    replace_engine::replace_engine(key_value_storage* kvs)
    : m_pimpl(new impl(kvs))
    {
        exact_handler eh(m_pimpl->m_exact);
        m_pimpl->m_kvs->list_keys(response_replace, eh);

        re_handler rh(m_pimpl->m_regex);
        m_pimpl->m_kvs->list_keys(response_replace_re, rh);
        for (const auto& f : rh.m_failed) {
            m_pimpl->m_kvs->drop(f, response_replace_re);
        }
    }
    // ---------------------------------------------------------------------------------------
    replace_engine::~replace_engine() = default;
    // ---------------------------------------------------------------------------------------
    bool replace_engine::empty() const
    {
        lock_t guard(m_pimpl->m_mtx);
        return m_pimpl->m_exact.empty() && m_pimpl->m_regex.empty();
    }
    // ---------------------------------------------------------------------------------------
    std::string replace_engine::find(const std::string& url) const
    {
        lock_t guard(m_pimpl->m_mtx);
        const std::string* key = nullptr;
        bool is_exact = true;
        if (m_pimpl->m_exact.find(url) != m_pimpl->m_exact.end()) {
            key = &url;
        } else {
            for (auto itr = m_pimpl->m_regex.begin(); itr != m_pimpl->m_regex.end(); itr++) {
                const std::regex& r = itr->second;
                std::smatch sm;
                std::regex_match(url, sm, r);
                if (!sm.empty())
                {
                    if (sm.length() == (int)url.size()) {
                        key = &itr->first;
                        is_exact = false;
                        break;
                    }
                }
            }
        }
        std::string text;
        if (key) {
            if (is_exact)
            {
                m_pimpl->m_kvs->get_with_meta(*key, response_replace, text);
            }
            else
            {
                m_pimpl->m_kvs->get_with_meta(*key, response_replace_re, text);
            }
        }
        return text;
    }
    // ---------------------------------------------------------------------------------------
    bool replace_engine::add(bool is_regexp, const std::string& url, const std::string& text) {
        lock_t guard(m_pimpl->m_mtx);

        if (!is_regexp) {
            m_pimpl->m_kvs->put(url, response_replace, text);
            m_pimpl->m_exact.insert(url);
        } else {
            try
            {
                m_pimpl->m_regex.emplace_back(url, create_re(url));
                m_pimpl->m_kvs->put(url, response_replace_re, text, false);
            } catch (...)
            {
                return false;
            }
        }
        return true;
    }
    // ---------------------------------------------------------------------------------------
    void replace_engine::remove(const std::string& url) {
        lock_t guard(m_pimpl->m_mtx);
        auto i = m_pimpl->m_exact.find(url);
        if (i != m_pimpl->m_exact.end()) {
            m_pimpl->m_exact.erase(i);
            m_pimpl->m_kvs->drop(url, response_replace, false);
        } else {
            auto itr = std::find_if(m_pimpl->m_regex.begin(), m_pimpl->m_regex.end(),
                                 [&url](const std::pair<std::string, std::regex>& t)
                                 { return t.first == url;}
                                 );
            if (itr != m_pimpl->m_regex.end()) {
                m_pimpl->m_regex.erase(itr);
                m_pimpl->m_kvs->drop(url, response_replace_re, false);
            }
        }
    }
}
// ==============================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>
#include "backend/work_dir.hh"

TEST_SUITE ("replace engine")
{
    TEST_CASE("simple_test") {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);
        binadox::replace_engine re(&kvs);

        REQUIRE(re.add(true, ".*", "AAA"));
        auto txt = re.find("https://example.com");
        REQUIRE(txt == "AAA");
    }

    TEST_CASE("simple_test 2") {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);
        binadox::replace_engine re(&kvs);

        REQUIRE(re.add(true, "https://example\\.com", "AAA"));
        auto txt = re.find("https://example.com");
        REQUIRE(txt == "AAA");
    }

    TEST_CASE("simple_test 3") {
        auto path = binadox::get_tmp_dir() / "kv_test.db";

        binadox::key_value_storage kvs(path);
        kvs.drop(false);
        binadox::replace_engine re(&kvs);

        REQUIRE(re.add(true, "https://example\\.com/[a-zA-Z0-9]+\\.js", "AAA"));
        auto txt = re.find("https://example.com");
        REQUIRE(txt.empty());

        txt = re.find("https://example.com/zopa.js");
        REQUIRE(txt == "AAA");
    }
}
#endif