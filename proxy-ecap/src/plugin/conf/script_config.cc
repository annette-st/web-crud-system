//
// Created by igor on 06/01/2021.
//

#include "script_config.hh"
#include "plugin/logger.hh"
namespace binadox
{
    script_config::script_config()
    : data_source_observer(data_source::eINJECTION_SCRIPT)
    {

    }
    // ----------------------------------------------------------------------------------------------
    void script_config::handle_data_source(data_source::type_t type, const data_source* ds)
    {
        if (type == data_source::eINJECTION_SCRIPT)
        {
            LOG_ERROR("Updating injection script");

            m_script = std::make_shared<script_data>(ds->get_injection_script(), 0);
        }
    }
    // ----------------------------------------------------------------------------------------------
    std::shared_ptr<script_data> script_config::get()
    {
        return m_script;
    }
}