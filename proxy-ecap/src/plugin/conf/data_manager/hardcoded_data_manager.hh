//
// Created by igor on 31/12/2020.
//

#ifndef BINADOX_ECAP_HARDCODED_DATA_MANAGER_HH
#define BINADOX_ECAP_HARDCODED_DATA_MANAGER_HH

#include "backend/data_manager.hh"
#include "plugin/conf/config_reader.hh"

class hardcoded_data_manager : public binadox::data_manager
{
public:
    hardcoded_data_manager(const binadox::config& config);

    std::unique_ptr<binadox::data_source> init() override;
private:
    binadox::config m_config;

};


#endif //BINADOX_ECAP_HARDCODED_DATA_MANAGER_HH
