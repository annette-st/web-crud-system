//
// Created by igor on 14/06/2021.
//

#ifndef BINADOX_ECAP_REPLACE_ENGINE_HH
#define BINADOX_ECAP_REPLACE_ENGINE_HH


#include <nonstd/string_view.hpp>
#include <memory>

namespace binadox
{
    class key_value_storage;

    class replace_engine
    {
    public:
        replace_engine(key_value_storage* kvs);
        ~replace_engine();

        std::string find(const std::string& url) const;
        bool add(bool is_regexp, const std::string& url, const std::string& text);
        void remove(const std::string& url);
        bool empty() const;
    private:
        struct impl;
        std::unique_ptr<impl> m_pimpl;
    };
} // ns binadox

#endif //BINADOX_ECAP_REPLACE_ENGINE_HH
