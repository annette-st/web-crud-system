//
// Created by igor on 06/01/2021.
//

#include "includes_config.hh"
#include "plugin/conf/includes_loader/ds_includes_loader.hh"
#include "plugin/logger.hh"

namespace binadox
{
    includes_config::includes_config()
    : data_source_observer(data_source::eLOGIN_INCLUDE_SCRIPTS, data_source::eON_LOAD_SCRIPT)
    {

    }
    // ----------------------------------------------------------------------------------------------------------
    void includes_config::handle_data_source(data_source::type_t type, const data_source* ds)
    {
        if (type == data_source::eLOGIN_INCLUDE_SCRIPTS)
        {
            LOG_TRACE("Updating login include scripts");
            m_logins = std::unique_ptr<abstract_includes_loader>(
                    new ds_includes_loader(ds->get_login_include_scripts()));
        }
        if (type == data_source::eON_LOAD_SCRIPT)
        {
            LOG_TRACE("Updating onload include scripts");
            m_onload = std::unique_ptr<abstract_includes_loader>(
                    new ds_includes_loader(ds->get_onload_scripts()));
        }
    }
    // ----------------------------------------------------------------------------------------------------------
    abstract_includes_loader& includes_config::get_logins()
    {
        return *m_logins;
    }
    // ----------------------------------------------------------------------------------------------------------
    abstract_includes_loader& includes_config::get_onload()
    {
        return *m_onload;
    }
}