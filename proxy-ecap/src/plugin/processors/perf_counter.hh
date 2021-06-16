//
// Created by igor on 17/08/2020.
//

#ifndef BINADOX_ECAP_PERF_COUNTER_HH
#define BINADOX_ECAP_PERF_COUNTER_HH

#include <map>
#include <ostream>
#include <string>
#include <mutex>

namespace binadox
{
    class perf_counter_data
    {
    public:
        perf_counter_data();
        ~perf_counter_data();

        void serialize(std::ostream& os) const;
        bool empty() const;
        void add(const std::string& uri, double us, bool processed);
        void swap(perf_counter_data& new_data);
    private:
        typedef std::pair<double, bool> value_t;
        typedef std::multimap<std::string, value_t> map_t;
        map_t data;
    };


    class perf_counter
    {
    public:
        perf_counter();
        ~perf_counter();

        void swap(perf_counter_data& new_data);
        void add(const std::string& uri, double us, bool processed);
    private:
        std::mutex mutex;
        perf_counter_data data;
    };

} // ns binadox

#endif //BINADOX_ECAP_PERF_COUNTER_HH
