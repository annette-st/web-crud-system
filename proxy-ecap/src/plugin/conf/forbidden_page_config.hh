//
// Created by igor on 06/01/2021.
//

#ifndef BINADOX_ECAP_FORBIDDEN_PAGE_CONFIG_HH
#define BINADOX_ECAP_FORBIDDEN_PAGE_CONFIG_HH

#include "backend/data_source.hh"
#include <string>

namespace binadox
{
    class forbidden_page_config : public data_source_observer
    {
    public:
        forbidden_page_config();

        void handle_data_source(data_source::type_t type, const data_source* ds) override;
        std::string get() const;
    private:
        std::string m_page;
    };
}


#endif //BINADOX_ECAP_FORBIDDEN_PAGE_CONFIG_HH
