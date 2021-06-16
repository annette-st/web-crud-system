//
// Created by igor on 30/07/2020.
//

#include <iterator>
#include <memory>

#include "backend/work_dir.hh"
#include "plugin/logger.hh"

#include "plugin/conf/configurator.hh"
#include "plugin/conf/config_reader.hh"
#include "plugin/conf/key_value_storage.hh"

#include "plugin/conf/data_manager/dynamic_data_manager.hh"
#include "plugin/conf/data_manager/hardcoded_data_manager.hh"

#include "plugin/conf/script_config.hh"
#include "plugin/conf/forbidden_page_config.hh"
#include "plugin/conf/includes_config.hh"

#include "plugin/filter/ignore_list.hh"
#include "plugin/filter/blacklist.hh"
#include "plugin/filter/tld.hh"

#include "plugin/ipc/factory.hh"
#include "plugin/ipc/sender_log.hh"

#include "plugin/processors/perf_counter.hh"

#include "plugin/sniffer_db/sniffer_ipc.hh"
#include "plugin/sniffer_db/sniffer_db.hh"

#include "backend/unused.hh"

#include "appgw/addons_registry.hh"
#include "plugin/filter/replace_engine.hh"

namespace binadox
{
    struct configurator_impl
    {
        configurator_impl(const config& cfg)
                : workspace(cfg.workspace_id),
                  path_to_script(cfg.path_to_script),
                  m_is_dynamic_config(false)
        {
            workdir = find_workdir(cfg);
            LOG_TRACE("Working directory", workdir);
            LOG_TRACE("Main script", path_to_script);

            if (cfg.use_dynamic_config)
            {
                m_is_dynamic_config = true;
                m_data_manager = std::unique_ptr<binadox::data_manager>(new dynamic_data_manager(cfg));
            }
            else
            {
                m_data_manager = std::unique_ptr<binadox::data_manager>(new hardcoded_data_manager(cfg));
            }
            m_data_manager->initialize();

            m_data_manager->notify_all(); // update all subscribers

            create_ipc_obj(cfg);
            create_kvs();
            create_sniffer_storage();
        }

        void create_kvs()
        {
            kvs = std::unique_ptr<key_value_storage>(new key_value_storage(workdir / "kvs.db"));
            m_replace_engine = std::unique_ptr<replace_engine>(new replace_engine(kvs.get()));
        }

        void create_sniffer_storage()
        {
            sniffer_storage = std::unique_ptr<sniffer_db>(new sniffer_db(new sniffer_ipc(ipc_obj)));
        }

        static std::string find_workdir(const config& cfg)
        {
            std::string wdir = cfg.work_dir;
            if (wdir.empty())
            {
                wdir = find_writable_location();
            }
            if (wdir.empty())
            {
                RAISE_EX("Workdir is empty");
            }
            if (!dir_exists(wdir))
            {
                if (!mk_path(wdir))
                {
                    RAISE_EX("Failed to create working directory", wdir);
                }
            }

            if (can_write(wdir))
            {
                return wdir;
            }

            RAISE_EX("Working directory", wdir, "is not writable");
            return "";
        }



        void create_ipc_obj(const config& cfg)
        {
            ipc* sq = new sender_log(workdir / "sq.db");

            if (cfg.ipc_data.type == "redis")
            {
                redis::connection_parameters cn;
                cn.username = cfg.ipc_data.username;
                cn.password = cfg.ipc_data.password;
                cn.host = cfg.ipc_data.hostname;
                cn.port = cfg.ipc_data.port;

                ipc_obj = std::shared_ptr<ipc>(create_ipc(sq, cn));
            }
        }
        fs::path workdir;



        std::string workspace;
        ignore_list url_filter;
        std::shared_ptr<ipc> ipc_obj;

        fs::path path_to_script;

        blacklist tld_blacklist;
        perf_counter performance_counter;
        std::string ip_to_sniff;
        std::mutex sniffer_mtx;

        std::unique_ptr<key_value_storage> kvs;
        std::unique_ptr<replace_engine> m_replace_engine;
        std::unique_ptr<sniffer_db> sniffer_storage;

        script_config m_script_config;
        forbidden_page_config m_403;
        includes_config m_includes;
        appgw::addons_registry m_addons_registry;


        std::unique_ptr<binadox::data_manager> m_data_manager;
        bool m_is_dynamic_config;
    };

    // ----------------------------------------------------------------------------------------------------
    configurator::configurator(const config& cfg)
    {
        pimpl = new configurator_impl(cfg);
    }
    // ----------------------------------------------------------------------------------------------------
    configurator::~configurator()
    {
        delete pimpl;
    }
    // ----------------------------------------------------------------------------------------------------
    std::shared_ptr<script_data> configurator::get_script() const
    {
        if (!pimpl->m_is_dynamic_config)
        {
            pimpl->m_script_config.handle_data_source(data_source::eINJECTION_SCRIPT, pimpl->m_data_manager->get_data_source().get());
        }

        return pimpl->m_script_config.get();
    }
    // ----------------------------------------------------------------------------------------------------
    std::string configurator::get_workspace() const
    {
        return pimpl->workspace;
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::accept_url(const libecap::Area& url, nonstd::string_view& domain) const
    {
        return !pimpl->url_filter.accept(url, domain);
    }
    // ----------------------------------------------------------------------------------------------------
    ipc* configurator::get_ipc()
    {
        return pimpl->ipc_obj.get();
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::has_blacklist() const
    {
        return !pimpl->tld_blacklist.empty();
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::is_blacklisted(const libecap::Area& url) const
    {
        return pimpl->tld_blacklist.accept(url.start, url.size);
    }
    // ----------------------------------------------------------------------------------------------------
    std::string configurator::get_blacklist_page(UNUSED_ARG const libecap::Area& url) const
    {
        return pimpl->m_403.get();
    }
    // ----------------------------------------------------------------------------------------------------
    perf_counter& configurator::get_perf_counter()
    {
        return pimpl->performance_counter;
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::need_to_sniff_from_ip(const libecap::Area& ip) const
    {
        nonstd::string_view sv(ip.start, ip.size);
        std::lock_guard<std::mutex> g(pimpl->sniffer_mtx);
        return (pimpl->ip_to_sniff == sv);
    }
    // ----------------------------------------------------------------------------------------------------
    void configurator::set_sniff_ip(const std::string& ip) const
    {
        std::lock_guard<std::mutex> g(pimpl->sniffer_mtx);
        if (ip.empty())
        {
            LOG_TRACE("Record traffic disabled");
        } else
        {
            LOG_TRACE("Recording traffic from ", ip);
        }
        pimpl->ip_to_sniff = ip;
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::is_sniffer_available() const
    {
        std::lock_guard<std::mutex> g(pimpl->sniffer_mtx);
        return !pimpl->ip_to_sniff.empty();
    }
    // ---------------------------------------------------------------------------------------------------
    key_value_storage& configurator::get_key_value_storage()
    {
        return *pimpl->kvs;
    }
    // ----------------------------------------------------------------------------------------------------
    sniffer_db& configurator::sniffer_database()
    {
        return *pimpl->sniffer_storage;
    }
    // ----------------------------------------------------------------------------------------------------
    abstract_includes_loader& configurator::login_config()
    {
        return pimpl->m_includes.get_logins();
    }
    // ----------------------------------------------------------------------------------------------------
    abstract_includes_loader& configurator::onload_config()
    {
        return pimpl->m_includes.get_onload();
    }
    // ----------------------------------------------------------------------------------------------------
    appgw::addons_registry& configurator::addons_registry()
    {
        return pimpl->m_addons_registry;
    }
    // ----------------------------------------------------------------------------------------------------
    replace_engine& configurator::get_replace_engine()
    {
        return *pimpl->m_replace_engine.get();
    }
    // ----------------------------------------------------------------------------------------------------
    static configurator* global_cfg = nullptr;
    void config_init(const config& cfg)
    {
        if (!global_cfg)
        {
            global_cfg = new configurator(cfg);
        }
    }
    // ----------------------------------------------------------------------------------------------------
    void config_done()
    {
        delete global_cfg;
        global_cfg = nullptr;
    }
    // ----------------------------------------------------------------------------------------------------
    configurator* get_config()
    {
        return global_cfg;
    }
    // ----------------------------------------------------------------------------------------------------
    data_manager& configurator::get_data_manager()
    {
        return *pimpl->m_data_manager;
    }
    // ----------------------------------------------------------------------------------------------------
    bool configurator::is_dynamic_config() const
    {
        return pimpl->m_is_dynamic_config;
    }
} // ns binadox

