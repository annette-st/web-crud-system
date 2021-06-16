#ifndef BINADOX_ECAP_ADDON_TOKEN_HH
#define BINADOX_ECAP_ADDON_TOKEN_HH

#include <string>

namespace binadox
{
    namespace appgw
    {
        class addon_token
        {
        public:
            addon_token();
            explicit addon_token(const std::string& v);

            bool wants_url() const;
            void wants_url(bool f);

            const std::string& accepted() const;
            void accepted(const std::string& v);
        private:
            bool m_wants_url;
            std::string m_accepted_by;
        };
    }
} // ns binadox

#endif //BINADOX_ECAP_ADDON_TOKEN_HH
