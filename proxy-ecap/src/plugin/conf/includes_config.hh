//
// Created by igor on 06/01/2021.
//

#ifndef BINADOX_ECAP_INCLUDES_CONFIG_HH
#define BINADOX_ECAP_INCLUDES_CONFIG_HH

#include <memory>

#include <backend/data_source.hh>
#include "plugin/conf/includes_loader/abstract_includes_loader.hh"

namespace binadox
{
    class includes_config : public data_source_observer
    {
    public:
        includes_config();

        void handle_data_source(data_source::type_t type, const data_source* ds) override;

        abstract_includes_loader& get_logins();
        abstract_includes_loader& get_onload();

    private:
        std::unique_ptr<abstract_includes_loader> m_onload;
        std::unique_ptr<abstract_includes_loader> m_logins;
    };
} // ns binadox


#endif //BINADOX_ECAP_INCLUDES_CONFIG_HH
