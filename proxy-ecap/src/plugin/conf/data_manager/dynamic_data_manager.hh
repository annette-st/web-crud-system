//
// Created by igor on 24/01/2021.
//

#ifndef BINADOX_ECAP_DYNAMIC_DATA_MANAGER_HH
#define BINADOX_ECAP_DYNAMIC_DATA_MANAGER_HH

#include "backend/data_manager.hh"
#include "plugin/conf/config_reader.hh"

namespace binadox
{
    class dynamic_data_manager : public binadox::data_manager
    {
    public:
        dynamic_data_manager(const binadox::config& config);

        std::unique_ptr<binadox::data_source> init() override;
    private:
        binadox::config m_config;

    };
} // ns binadox
#endif //BINADOX_ECAP_DYNAMIC_DATA_MANAGER_HH
