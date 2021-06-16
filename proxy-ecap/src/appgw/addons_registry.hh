#ifndef BINADOX_ECAP_ADDONS_REGISTRY_HH
#define BINADOX_ECAP_ADDONS_REGISTRY_HH

#include <map>
#include <mutex>
#include <memory>
#include <vector>

#include "appgw/js_addon.hh"
#include "appgw/addon_token.hh"
#include "backend/data_source.hh"

namespace binadox
{
    namespace appgw
    {
        class addons_registry : public data_source_observer
        {
        public:
            addons_registry();

            addon_token wants_url(const nonstd::string_view& url);
            addon_token accept(const nonstd::string_view& method,
                               const nonstd::string_view& url,
                               const std::map<std::string, std::string>& headers);

            bool accept(const addon_token& tk, const nonstd::string_view& method,
                        const nonstd::string_view& url,
                        const std::map<std::string, std::string>& headers);

            bool empty() const;

            void handle_data_source(data_source::type_t type, const data_source* ds) override;
        private:
            mutable std::mutex m_mtx;
            std::map<std::string, std::unique_ptr<js_addon>> m_addons;
        };
    } // namespace appgw
} // namespace binadox

#endif //BINADOX_ECAP_ADDONS_REGISTRY_HH
