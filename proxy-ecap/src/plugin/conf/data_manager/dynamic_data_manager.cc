//
// Created by igor on 24/01/2021.
//

#include "dynamic_data_manager.hh"
#include "plugin/conf/403.hh"
#include "backend/fs.hh"

namespace binadox
{
    class dynamic_data_source : public binadox::data_source
    {
    public:
        dynamic_data_source(const binadox::config& cfg);

        list_of_strings get_ignore_list() const override;
        void set_ignore_list(const list_of_strings& data) override;

        list_of_strings get_black_list() const override;
        void set_black_list(const list_of_strings& data) override;

        void set_login_include_scripts(const map_of_strings& v) override;
        map_of_strings get_login_include_scripts() const override;

        void set_onload_scripts(const map_of_strings& v) override;
        map_of_strings get_onload_scripts() const override;

        void set_injection_script(const std::string& v) override;
        std::string get_injection_script() const override;

        std::string get_403_page() const override;

        void set_addons_list(const list_of_strings& v) override;
        list_of_strings get_addons_list() const override;

        void clear(type_t what);
    private:
        fs::path path_to_script;
        fs::path path_to_logins;
        fs::path path_to_onload;

        list_of_strings m_black_list;
        list_of_strings m_ignore_list;
        list_of_strings m_addons_list;
        map_of_strings  m_login_include_scripts;
        map_of_strings  m_onload_scripts;
        std::string     m_injection_script;
    };

// -------------------------------------------------------------------------------------------------------
    dynamic_data_source::dynamic_data_source(const binadox::config& cfg)
            : path_to_script(cfg.path_to_script),
              path_to_logins(cfg.path_to_logins),
              path_to_onload(cfg.path_to_onload)
    {

    }
    // -------------------------------------------------------------------------------------------------------
    dynamic_data_source::list_of_strings dynamic_data_source::get_ignore_list() const
    {
        return m_ignore_list;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_ignore_list(const list_of_strings& data)
    {
        m_ignore_list = data;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_black_list(const list_of_strings& data)
    {
        m_black_list = data;
    }
    // -------------------------------------------------------------------------------------------------------
    dynamic_data_source::list_of_strings dynamic_data_source::get_black_list() const
    {
        return m_black_list;
    }
    // -------------------------------------------------------------------------------------------------------
    dynamic_data_source::map_of_strings dynamic_data_source::get_login_include_scripts() const
    {
        return m_login_include_scripts;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_login_include_scripts(const map_of_strings& v)
    {
        m_login_include_scripts = v;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_onload_scripts(const map_of_strings& v)
    {
        m_onload_scripts = v;
    }
    // -------------------------------------------------------------------------------------------------------
    dynamic_data_source::map_of_strings dynamic_data_source::get_onload_scripts() const
    {
        return m_onload_scripts;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_injection_script(const std::string& v)
    {
        m_injection_script = v;
    }
    // -------------------------------------------------------------------------------------------------------
    std::string dynamic_data_source::get_injection_script() const
    {
        return m_injection_script;
    }
    // -------------------------------------------------------------------------------------------------------
    std::string dynamic_data_source::get_403_page() const
    {
        return binadox::page403;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::set_addons_list(const list_of_strings& v)
    {
        m_addons_list = v;
    }
    // -------------------------------------------------------------------------------------------------------
    dynamic_data_source::list_of_strings dynamic_data_source::get_addons_list() const
    {
        return m_addons_list;
    }
    // -------------------------------------------------------------------------------------------------------
    void dynamic_data_source::clear(type_t what)
    {
        switch (what)
        {
            case data_source::eBLACK_LIST:
                m_black_list.clear();
                break;
            case data_source::eIGNORE_LIST:
                m_ignore_list.clear();
                break;
            case data_source::eADDONS_LIST:
                m_addons_list.clear();
                break;
            case data_source::eON_LOAD_SCRIPT:
                m_onload_scripts.clear();
                break;
            case data_source::eLOGIN_INCLUDE_SCRIPTS:
                m_login_include_scripts.clear();
                break;
            case data_source::eINJECTION_SCRIPT:
                m_injection_script.clear();
                break;
            default:
                data_source::clear(what);
        }
    }
    // ==================================================================================================
    dynamic_data_manager::dynamic_data_manager(const binadox::config& config)
            : m_config(config)
    {

    }
    // --------------------------------------------------------------------------------------------------
    std::unique_ptr<binadox::data_source> dynamic_data_manager::init()
    {
        return std::unique_ptr<binadox::data_source>(new dynamic_data_source(m_config));
    }

} // ns binadox