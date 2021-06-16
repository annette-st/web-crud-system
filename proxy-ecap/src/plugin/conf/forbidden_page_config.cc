//
// Created by igor on 06/01/2021.
//

#include "forbidden_page_config.hh"

namespace binadox
{
    forbidden_page_config::forbidden_page_config()
    : data_source_observer(data_source::e403_PAGE)
    {

    }
    // ------------------------------------------------------------------------------------
    void forbidden_page_config::handle_data_source(data_source::type_t type, const data_source* ds)
    {
        if (type == data_source::e403_PAGE)
        {
            m_page = ds->get_403_page();
        }
    }
    // ------------------------------------------------------------------------------------
    std::string forbidden_page_config::get() const
    {
        return m_page;
    }
}