#include "addon_token.hh"

namespace binadox
{
    namespace appgw
    {
        addon_token::addon_token()
        : m_wants_url(false)
        {

        }
        // --------------------------------------------------------------------------------
        addon_token::addon_token(const std::string& v)
        : m_wants_url(true),
          m_accepted_by(v)
        {

        }
        // --------------------------------------------------------------------------------
        bool addon_token::wants_url() const
        {
            return m_wants_url;
        }
        // --------------------------------------------------------------------------------
        void addon_token::wants_url(bool f)
        {
            m_wants_url = f;
        }
        // --------------------------------------------------------------------------------
        const std::string& addon_token::accepted() const
        {
            return m_accepted_by;
        }
        // --------------------------------------------------------------------------------
        void addon_token::accepted(const std::string& v)
        {
            m_accepted_by = v;
        }
    } // ns appgw
} // ns binadox
