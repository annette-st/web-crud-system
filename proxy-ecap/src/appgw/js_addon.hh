#ifndef BINADOX_ECAP_JS_ADDON_HH
#define BINADOX_ECAP_JS_ADDON_HH

#include <string>
#include <memory>
#include <map>
#include <backend/fs.hh>
#include <nonstd/string_view.hpp>

namespace binadox
{
    namespace appgw
    {
        class js_addon_impl;
        class js_addon
        {
        public:
            static js_addon from_text(const std::string& text);
            static js_addon from_file(const fs::path& path);
        public:
            ~js_addon();
            js_addon(js_addon&& other) noexcept;

            js_addon(const js_addon&) = delete;
            js_addon& operator = (const js_addon&) = delete;

            const std::string& name() const;
            bool wants_url(const nonstd::string_view& url);
            bool accept(const nonstd::string_view& method,
                        const nonstd::string_view& url,
                        const std::map<std::string, std::string>& headers);
        private:
            explicit js_addon(const std::string& js_txt);
        private:
            std::unique_ptr<js_addon_impl> m_pimpl;
        };
    } // ns appgw
} // ns binadox

#endif //BINADOX_ECAP_JS_ADDON_HH
