//
// Created by igor on 31/12/2020.
//

#include "data_source.hh"
#include "data_manager.hh"
#include "unused.hh"

namespace binadox
{
    data_source::~data_source()
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_ignore_list(UNUSED_ARG const list_of_strings& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_black_list(UNUSED_ARG const list_of_strings& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_login_include_scripts(UNUSED_ARG const map_of_strings& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_onload_scripts(UNUSED_ARG const map_of_strings& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_injection_script(UNUSED_ARG const std::string& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_403_page(UNUSED_ARG const std::string& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::set_addons_list(UNUSED_ARG const list_of_strings& data)
    {

    }
    // --------------------------------------------------------------------------------
    void data_source::clear(type_t what)
    {
        static list_of_strings  empty_list;
        static map_of_strings   empty_map;
        static std::string      empty_string;
        switch (what)
        {
            case eIGNORE_LIST:
                set_ignore_list(empty_list);
                break;
            case eLOGIN_INCLUDE_SCRIPTS:
                set_login_include_scripts(empty_map);
                break;
            case eON_LOAD_SCRIPT:
                set_onload_scripts(empty_map);
                break;
            case eINJECTION_SCRIPT:
                set_injection_script(empty_string);
                break;
            case e403_PAGE:
                set_403_page(empty_string);
                break;
            case eBLACK_LIST:
                set_black_list(empty_list);
                break;
            case eADDONS_LIST:
                set_addons_list(empty_list);
                break;
        }
    }
    // ===============================================================================
    void data_source_observer::register_me (data_source::type_t type)
    {
        m_types.push_back(type);
        data_manager::register_observer(type, this);
    }
    // --------------------------------------------------------------------------------
    data_source_observer::~data_source_observer()
    {
        for (auto t : m_types)
        {
            data_manager::unregister_observer(t, this);
        }
    }


}