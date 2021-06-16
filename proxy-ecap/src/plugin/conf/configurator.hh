//
// Created by igor on 30/07/2020.
//

#ifndef BINADOX_ECAP_CONFIGURATOR_HH
#define BINADOX_ECAP_CONFIGURATOR_HH

#include <string>
#include <libecap/common/area.h>
#include <nonstd/string_view.hpp>
#include <memory>

#include "plugin/ipc/ipc.hh"
#include "plugin/pipeline/script/script_data.hh"
#include "backend/data_manager.hh"

namespace binadox
{
    struct config;
    struct configurator_impl;
    class perf_counter;
    class ipc;
    class key_value_storage;
    class sniffer_db;
    class abstract_includes_loader;
    class replace_engine;

    namespace appgw
    {
        class addons_registry;
    } // ns appgw

    class configurator;

    void config_init(const config& cfg);
    void config_done();
    configurator* get_config();

    class configurator
    {
        friend void config_init(const config& cfg);
        friend void config_done();
    public:
        std::shared_ptr<script_data> get_script() const;
        std::string get_workspace() const;
        bool accept_url(const libecap::Area& url, nonstd::string_view& domain) const;
        ipc* get_ipc();

        bool has_blacklist() const;
        bool is_blacklisted(const libecap::Area& url) const;
        std::string get_blacklist_page(const libecap::Area& url) const;

        data_manager& get_data_manager();

        perf_counter& get_perf_counter();

        bool need_to_sniff_from_ip(const libecap::Area& ip) const;
        void set_sniff_ip(const std::string& ip) const;
        bool is_sniffer_available() const;

        key_value_storage& get_key_value_storage();
        sniffer_db& sniffer_database();

        abstract_includes_loader& login_config();
        abstract_includes_loader& onload_config();
        appgw::addons_registry& addons_registry();

        bool is_dynamic_config() const;

        replace_engine& get_replace_engine();
    private:
        configurator(const config& cfg);
        ~configurator();
    private:
        configurator_impl* pimpl;
    };


}

#endif //BINADOX_ECAP_CONFIGURATOR_HH
