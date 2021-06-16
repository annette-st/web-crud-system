//
// Created by igor on 16/08/2020.
//

#ifndef BINADOX_ECAP_BLACKLIST_HH
#define BINADOX_ECAP_BLACKLIST_HH

#include <cstddef>
#include <string>
#include "backend/data_source.hh"

namespace binadox
{
    struct blacklist_impl;

    class blacklist : public data_source_observer
    {
    public:
        blacklist();
        ~blacklist();

        bool empty() const;
        bool accept(const char* url, size_t len) const;

        void clear ();
        void insert(const std::string& pattern);

        void handle_data_source(data_source::type_t type, const data_source* ds) override;
    private:
        blacklist(const blacklist&);
        blacklist& operator = (const blacklist&);
    private:
        blacklist_impl* pimpl;
    };
} // ns binadox

#endif //BINADOX_ECAP_BLACKLIST_HH
