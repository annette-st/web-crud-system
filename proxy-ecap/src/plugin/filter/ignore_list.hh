//
// Created by igor on 19/07/2020.
//

#ifndef BINADOX_ECAP_IGNORE_LIST_HH
#define BINADOX_ECAP_IGNORE_LIST_HH

#include <libecap/common/area.h>
#include <nonstd/string_view.hpp>
#include "plugin/filter/trie.hh"
#include "backend/data_source.hh"

namespace binadox
{
    class ignore_list : public data_source_observer
    {
    public:
        ignore_list();
        ~ignore_list();

        bool accept(const libecap::Area& url, nonstd::string_view & domain) const;

        void handle_data_source(data_source::type_t type, const data_source* ds);
    private:
        trie filter;
    };
} // ns binadox


#endif //BINADOX_ECAP_IGNORE_LIST_HH
