//
// Created by igor on 26/01/2021.
//

#include <map>
#include <mutex>

#include <nlohmann/json.hpp>

#include "message_parser.hh"

#include "plugin/conf/configurator.hh"
#include "plugin/filter/tld.hh"
#include "plugin/logger.hh"
#include "backend/unused.hh"

namespace binadox
{
    namespace detail
    {
        class parser_registry
        {
        public:
            parser_registry();

            void parse(const nlohmann::json& message) const;
        private:
            static void handle_blacklist_update(const nlohmann::json& message, configurator* cfg);
            static void handle_ignorelist_update(const nlohmann::json& message, configurator* cfg);
            static void handle_record_traffic(const nlohmann::json& message, configurator* cfg);
            static void handle_stop_record_traffic(const nlohmann::json& message, configurator* cfg);
            static void handle_script_update(const nlohmann::json& message, configurator* cfg);
        private:
            using message_handler_t = void (*)(const nlohmann::json&, configurator*);
            using handler_map_t = std::map<std::string, message_handler_t>;

            handler_map_t m_handlers;
            mutable std::mutex m_mtx;
        };

        // ---------------------------------------------------------------------------------------------
        parser_registry::parser_registry()
        {
            m_handlers["blacklist-update"] = handle_blacklist_update;
            m_handlers["ignore-list-update"] = handle_ignorelist_update;
            m_handlers["record-traffic"] = handle_record_traffic;
            m_handlers["stop-record-traffic"] = handle_stop_record_traffic;
            m_handlers["script-update"] = handle_script_update;
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::handle_script_update(const nlohmann::json& message, configurator* cfg)
        {
            if (message.contains("value"))
            {
                const auto& value = message["value"];
                if (value.is_array())
                {
                    data_source::list_of_strings addon_scripts;
                    data_source::map_of_strings login_include_scripts;
                    data_source::map_of_strings onload_scripts;
                    std::string injection_script;

                    for (auto it = value.begin(); it != value.end(); it++)
                    {
                        const auto& obj = *it;
                        if (obj.is_object())
                        {
                            if (obj.contains("type") && obj.contains("script"))
                            {
                                const std::string script_type = obj["type"];
                                const std::string script = obj["script"];
                                if (script_type == "sniffer_script" || script_type == "proxy_sniffer")
                                {
                                    LOG_TRACE("Adding addon script ", obj["scriptName"]);
                                    addon_scripts.push_back(script);
                                }
                                else
                                {
                                    if (script_type == "login_include_script")
                                    {
                                        if (obj.contains("scriptName"))
                                        {
                                            const std::string name = obj["scriptName"];
                                            login_include_scripts[name] = script;
                                        }
                                    }
                                    else
                                    {
                                        if (script_type == "injection_script")
                                        {
                                            injection_script = script;
                                        }
                                        else
                                        {
                                            if (script_type == "onload_script")
                                            {
                                                if (obj.contains("scriptName"))
                                                {
                                                    const std::string name = obj["scriptName"];
                                                    onload_scripts[name] = script;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }



                    if (!addon_scripts.empty())
                    {

                        cfg->get_data_manager().clear(data_source::eADDONS_LIST);
                        cfg->get_data_manager().update<data_source::eADDONS_LIST>(addon_scripts);
                    }
                    if (!login_include_scripts.empty())
                    {
                        cfg->get_data_manager().clear(data_source::eLOGIN_INCLUDE_SCRIPTS);
                        cfg->get_data_manager().update<data_source::eLOGIN_INCLUDE_SCRIPTS>(login_include_scripts);
                    }
                    if (!onload_scripts.empty())
                    {
                        cfg->get_data_manager().clear(data_source::eON_LOAD_SCRIPT);
                        cfg->get_data_manager().update<data_source::eON_LOAD_SCRIPT>(onload_scripts);
                    }
                    if (!injection_script.empty())
                    {
                        cfg->get_data_manager().clear(data_source::eINJECTION_SCRIPT);
                        cfg->get_data_manager().update<data_source::eINJECTION_SCRIPT>(injection_script);
                    }

                }
                else
                {
                    LOG_ERROR("script-update message value field should be array");
                }
            }
            else
            {
                LOG_ERROR("script-update message missing value field");
            }
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::handle_stop_record_traffic(UNUSED_ARG const nlohmann::json& message, configurator* cfg)
        {
            cfg->set_sniff_ip("");
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::handle_record_traffic(const nlohmann::json& message, configurator* cfg)
        {
            if (message.contains("from-ip"))
            {
                const std::string record_from_ip = message["from-ip"];
                cfg->set_sniff_ip(record_from_ip);
            } else
            {
                LOG_ERROR("from-ip field is missing");
            }
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::handle_ignorelist_update(const nlohmann::json& message, configurator* cfg)
        {
            if (message.contains("value"))
            {
                const nlohmann::json& value = message["value"];

                cfg->get_data_manager().clear(data_source::eIGNORE_LIST);
                data_source::list_of_strings list_data;

                LOG_TRACE("Applying ignore list of size ", value.size());

                for (auto it = value.begin(); it != value.end(); it++)
                {
                    list_data.push_back(it->get<std::string>());
                }
                cfg->get_data_manager().update<data_source::eIGNORE_LIST>(list_data);
            }
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::handle_blacklist_update(const nlohmann::json& message, configurator* cfg)
        {
            if (message.contains("value"))
            {
                const nlohmann::json& value = message["value"];

                cfg->get_data_manager().clear(data_source::eBLACK_LIST);
                data_source::list_of_strings list_data;

                LOG_TRACE("Applying blacklist of size ", value.size());

                for (auto it = value.begin(); it != value.end(); it++)
                {
                    auto tld = get_tld_from_url(it->get<std::string>());
                    if (!tld.empty())
                    {
                        list_data.push_back(tld);
                    }
                }
                cfg->get_data_manager().update<data_source::eBLACK_LIST>(list_data);
            }
        }
        // ---------------------------------------------------------------------------------------------
        void parser_registry::parse(const nlohmann::json& message) const
        {
            auto* cfg = get_config();
            if (!cfg)
            {
                return;
            }
            if (!message.contains("message"))
            {
                LOG_ERROR("No message field found");
                return;
            }

            const std::string type = message["message"];

            LOG_TRACE("Parsing message of type", type);

            auto itr = m_handlers.find(type);
            if (itr != m_handlers.end())
            {
                std::lock_guard<std::mutex> g(m_mtx);
                itr->second(message, cfg);
            } else
            {
                throw std::runtime_error("Unknown message type " + type);
            }
        }
    }
    // =================================================================================================
    void parse_ipc_message(const char* input)
    {
        if (!input)
        {
            return;
        }
        static detail::parser_registry parser;
        try
        {
            auto message = nlohmann::json::parse(input);

            if (!message.contains("message"))
            {
                LOG_ERROR("No message field found");
                return;
            }
            parser.parse(message);
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Failed to parse incoming IPC message: ", e.what());
        }
    }
}