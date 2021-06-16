//
// Created by igor on 06/01/2021.
//

#ifndef BINADOX_ECAP_SCRIPT_CONFIG_HH
#define BINADOX_ECAP_SCRIPT_CONFIG_HH

#include <memory>
#include "backend/data_source.hh"
#include "plugin/pipeline/script/script_data.hh"

namespace binadox
{
    class script_config : public data_source_observer
    {
    public:
        script_config();

        void handle_data_source(data_source::type_t type, const data_source* ds) override;
        std::shared_ptr<script_data> get();
    private:
        std::shared_ptr<script_data> m_script;
    };
} // ns binadox

#endif //BINADOX_ECAP_SCRIPT_CONFIG_HH
