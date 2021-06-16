#include "addons_registry.hh"

namespace binadox
{
    namespace appgw
    {
        addons_registry::addons_registry()
        : data_source_observer(data_source::eADDONS_LIST)
        {

        }
        // ----------------------------------------------------------------------------------------------
        addon_token addons_registry::wants_url(const nonstd::string_view& url)
        {
            std::lock_guard<std::mutex> g(m_mtx);
            for (auto& kv : m_addons)
            {
                if (kv.second->wants_url(url))
                {
                    return addon_token(kv.first);
                }
            }
            return addon_token();
        }
        // ----------------------------------------------------------------------------------------------
        addon_token addons_registry::accept(const nonstd::string_view& method,
                                            const nonstd::string_view& url,
                                            const std::map<std::string, std::string>& headers)
        {
            std::lock_guard<std::mutex> g(m_mtx);
            for (auto& kv : m_addons)
            {
                if (kv.second->accept(method, url, headers))
                {
                    return addon_token(kv.first);
                }
            }
            return addon_token();
        }
        // ----------------------------------------------------------------------------------------------
        bool addons_registry::accept(const addon_token& tk, const nonstd::string_view& method,
                                     const nonstd::string_view& url,
                                     const std::map<std::string, std::string>& headers)
        {
            std::lock_guard<std::mutex> g(m_mtx);
            auto itr = m_addons.find(tk.accepted());
            if (itr != m_addons.end())
            {
                return itr->second->accept(method, url, headers);
            }
            return false;
        }
        // ----------------------------------------------------------------------------------------------
        bool addons_registry::empty() const
        {
            std::lock_guard<std::mutex> g(m_mtx);
            return m_addons.empty();
        }
        // ----------------------------------------------------------------------------------------------
        void addons_registry::handle_data_source(data_source::type_t type, const data_source* ds)
        {
            if (type == data_source::eADDONS_LIST)
            {
                std::lock_guard<std::mutex> g(m_mtx);
                m_addons.clear();
                for (const auto& text : ds->get_addons_list())
                {
                    try
                    {
                        std::unique_ptr<js_addon> addon (new js_addon(std::move(js_addon::from_text(text))));
                        auto name = addon->name();
                        m_addons.emplace(std::make_pair(name, std::move(addon)));
                    } catch (std::exception& e)
                    {
                        // TODO
                    }
                }
            }
        }
    } // ns appgw
} // ns binadox
