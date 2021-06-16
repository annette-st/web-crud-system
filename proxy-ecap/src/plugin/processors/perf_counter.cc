//
// Created by igor on 17/08/2020.
//

#include "plugin/processors/perf_counter.hh"
#include "plugin/filter/tld.hh"
#include <nlohmann/json.hpp>

namespace binadox
{
    perf_counter_data::perf_counter_data()
    {

    }
    // ---------------------------------------------------------------------------------------------
    perf_counter_data::~perf_counter_data()
    {

    }
    // ---------------------------------------------------------------------------------------------
    void perf_counter_data::serialize(std::ostream& os) const
    {
        nlohmann::json obj = nlohmann::json::array();
        std::pair<map_t::const_iterator, map_t::const_iterator> range;
        for(map_t::const_iterator i = data.begin(); i != data.end(); i = range.second)
        {
            // Get the range of the current key
            range = data.equal_range(i->first);
            if (range.first != range.second)
            {
                nlohmann::json array = nlohmann::json::array();
                for (map_t::const_iterator d = range.first; d != range.second; ++d)
                {
                    const double us = d->second.first;
                    nlohmann::json x = nlohmann::json::object();
                    x["time"] = us;
                    x["proc"] = d->second.second;
                    array.push_back(x);
                }
                const std::string& uri = range.first->first;
                nlohmann::json counter = nlohmann::json::object();
                counter["domain"] = uri;
                counter["data"] = array;
                obj.push_back(counter);
            }
        }
        nlohmann::json message = nlohmann::json::object();
        message["type"] = "perf_data";
        message["payload"] = obj;

        os << message;
    }
    // ---------------------------------------------------------------------------------------------
    bool perf_counter_data::empty() const
    {
        return data.empty();
    }
    // ---------------------------------------------------------------------------------------------
    void perf_counter_data::add(const std::string& uri, double us, bool processed)
    {
        data.insert(map_t::value_type(uri, value_t(us, processed)));
    }
    // ---------------------------------------------------------------------------------------------
    void perf_counter_data::swap(perf_counter_data& new_data)
    {
        data.swap(new_data.data);
    }
    // =============================================================================================
    perf_counter::perf_counter()
    {

    }
    // --------------------------------------------------------------------------------------------
    perf_counter::~perf_counter()
    {

    }
    // --------------------------------------------------------------------------------------------
    void perf_counter::swap(perf_counter_data& new_data)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.swap(new_data);
    }
    // --------------------------------------------------------------------------------------------
    void perf_counter::add(const std::string& uri, double us, bool processed)
    {

        std::lock_guard<std::mutex> lock(mutex);
        data.add(get_domain(uri, false), us, processed);
    }
} // ns binadox

#if defined(BINADOX_WITH_UNITTESTS)
#include <doctest.h>
#include <sstream>

#define EXPECTED_JSON "{\"payload\":[{\"data\":[{\"proc\":true,\"time\":1.0},{\"proc\":false,\"time\":10.0}],\"domain\":\"a\"},{\"data\":[{\"proc\":false,\"time\":101.0}],\"domain\":\"b\"}],\"type\":\"perf_data\"}"
#define EXPECTED_JSON_NULL "{\"payload\":[],\"type\":\"perf_data\"}"
TEST_SUITE("perf-data")
{
    TEST_CASE("serialize")
    {
        binadox::perf_counter_data d;
        d.add("a", 1, true);
        d.add("a", 10, false);
        d.add("b", 101, false);

        std::ostringstream os;
        d.serialize(os);
        REQUIRE(os.str() == EXPECTED_JSON);
    }

    TEST_CASE("swap")
    {
        binadox::perf_counter_data d;
        d.add("a", 1, true);
        d.add("a", 10, false);
        d.add("b", 101, false);

        binadox::perf_counter_data d1;
        d.swap(d1);
        {
            std::ostringstream os;
            d1.serialize(os);
            REQUIRE(os.str() == EXPECTED_JSON);

        }
        {
            std::ostringstream os;
            d.serialize(os);
            REQUIRE(os.str() == EXPECTED_JSON_NULL);
        }
    }
}

#endif
