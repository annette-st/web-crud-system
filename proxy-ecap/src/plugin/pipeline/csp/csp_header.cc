//
// Created by igor on 20/07/2020.
//

#include <sstream>
#include "plugin/pipeline/csp/csp_header.hh"
#include "plugin/pipeline/csp/csp_scanner.h"
#include "plugin/string_utils.hh"



namespace binadox
{
    const std::string csp_header::SCRIPT_SRC = "script-src";
    const std::string csp_header::DEFAULT_SRC = "default-src";
    const std::string csp_header::FRAME_ANCSETORS = "frame-ancestors";

    std::string csp_header::NONCE = "'nonce-";
    std::string csp_header::SHA256  = "'sha256-";
    std::string csp_header::SELF = "'self'";
    std::string csp_header::NONE = "'none'";
    // -------------------------------------------------------------------------------------
    namespace detail
    {
        class builder
        {
        public:
            builder(csp_header& obj, csp_scanner& ascanner)
            : ret(obj),
            scanner(ascanner)
            {

            }

            void run()
            {
                while (csp_scan(&scanner))
                {

                }
            }

        private:
            csp_header& ret;
            csp_scanner& scanner;
            std::string current_directive_name;
            csp_header::directive   current_entries;
        };
    }
    // -------------------------------------------------------------------------------------
    csp_header csp_header::parse(const char* header_begin, const char* header_end)
    {
        csp_header ret;
        csp_scanner scanner;
        csp_scanner_init(&scanner, header_begin, header_end);

        std::string current_directive_name;
        directive   current_entries;
        while (csp_scan(&scanner))
        {
            if (scanner.token_type == CSP_TOKEN)
            {
                std::string token (scanner.token_begin, scanner.token_end);

                if (current_directive_name.empty())
                {
                    current_directive_name = token;
                }
                else
                {
                    current_entries.entries.push_back(directive_entry(token));
                }
            } else
            {
                /*
                 * Make sure that you list all required resources of a specific type in a single directive.
                 * If you wrote something like script-src https://host1.com; script-src https://host2.com
                 * the second directive would simply be ignored
                 */
                if (ret.directives[current_directive_name].entries.empty())
                {
                    std::swap(ret.directives[current_directive_name], current_entries);
                    ret.order.push_back(current_directive_name);
                }
                current_directive_name.clear();
            }
        }
        if (!current_directive_name.empty())
        {
            std::swap(ret.directives[current_directive_name], current_entries);
            ret.order.push_back(current_directive_name);
            ret.has_last_semi = false;
        } else {
            ret.has_last_semi = true;
        }
        return ret;
    }
    // --------------------------------------------------------------------------------
    csp_header::csp_header()
    {

    }
    // --------------------------------------------------------------------------------
    std::string csp_header::to_string() const
    {
        std::ostringstream os;
        bool is_first = true;
        bool is_empty = true;
        for (string_vec_t::const_iterator i = order.begin(); i != order.end(); i++)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                os << "; ";
            }
            is_empty = false;
            os << *i << " ";
            directives_t::const_iterator j = directives.find(*i);
            bool first_e = true;
            for (entries_t::const_iterator e = j->second.entries.begin(); e != j->second.entries.end(); e++)
            {
                if (first_e)
                {
                    first_e = false;
                }
                else
                {
                    os << " ";
                }
                os << e->value;
            }
        }
        if (has_last_semi)
        {
            if (!is_empty)
            {
                os << ";";
            }
        }
        return os.str();
    }
    // -------------------------------------------------------------------------------------------------
    csp_header::nonce_info_t csp_header::get_nonce_policy() const
    {
        nonce_policy_t policy = eNONE;
        std::string value = "";
        if (!get_nonce_policy(SCRIPT_SRC, policy, value))
        {
            get_nonce_policy(DEFAULT_SRC, policy, value);
        }
        return nonce_info_t (policy, value);
    }
    // -------------------------------------------------------------------------------------------------
    bool csp_header::get_nonce_policy(const std::string& section, nonce_policy_t& policy, std::string& value) const
    {
        directives_t::const_iterator itr = directives.find(section);
        if (itr == directives.end())
        {
            return false;
        }
        policy = eNONE;
        for (entries_t::const_iterator i = itr->second.entries.begin(); i != itr->second.entries.end(); i++)
        {
            if (starts_with(i->value, SHA256))
            {
                policy = eSHA;
                value = i->value.substr(SHA256.size(), i->value.size() - SHA256.size() - 1);
                return true;
            }
            if (starts_with(i->value, NONCE))
            {
                policy = ePRECALCULATED;
                value = i->value.substr(NONCE.size(), i->value.size() - NONCE.size() - 1);
                return true;
            }
            if (starts_with(i->value, SELF))
            {
                policy = eSELF;
                value = "";
            }
            if (starts_with(i->value, NONE))
            {
                policy = eDISABLED;
                value = "";
            }
        }

        return policy != eNONE;
    }
    // -------------------------------------------------------------------------------------------
    void csp_header::add_nonce(const std::string& value)
    {
        directives_t::const_iterator itr = directives.find(SCRIPT_SRC);
        if (itr == directives.end())
        {
            order.push_back(SCRIPT_SRC);
        }
        directives[SCRIPT_SRC].entries.push_back(directive_entry("'nonce-" + value+"'"));
    }
    // -------------------------------------------------------------------------------------------
    void csp_header::add_self()
    {
        directives_t::const_iterator itr = directives.find(SCRIPT_SRC);
        if (itr == directives.end())
        {
            order.push_back(SCRIPT_SRC);
        }

        directives[SCRIPT_SRC].entries.push_back(directive_entry(SELF));
    }
    // -------------------------------------------------------------------------------------------
    void csp_header::remove_frame_ancestors()
    {
        directives_t::iterator itr = directives.find(FRAME_ANCSETORS);
        if (itr != directives.end())
        {
            directives.erase(itr);
            for (auto i = order.begin(); i!=order.end(); i++)
            {
                if (*i == FRAME_ANCSETORS)
                {
                    order.erase(i);
                    break;
                }
            }
        }
    }
    // -------------------------------------------------------------------------------------------
    bool csp_header::is_self_allowed() const
    {
        return has_keyword(SCRIPT_SRC, SELF) || has_keyword(DEFAULT_SRC, SELF);
    }
    // -------------------------------------------------------------------------------------------
    bool csp_header::has_section (const std::string& section) const
    {
        directives_t::const_iterator itr = directives.find(section);
        if (itr == directives.end())
        {
            return false;
        }
        return true;
    }
    // -------------------------------------------------------------------------------------------
    bool csp_header::has_keyword(const std::string& section, const std::string& keyword) const
    {
        directives_t::const_iterator itr = directives.find(section);
        if (itr == directives.end())
        {
            return false;
        }
        for (entries_t::const_iterator i = itr->second.entries.begin(); i != itr->second.entries.end(); i++)
        {
            if (ends_with(i->value, ":"))
            {
                return false;
            }
            if (i->value == keyword)
            {
                return true;
            }
        }
        return false;
    }

} // ns binadox

// ====================================================================================
// UNITTEST
// ====================================================================================

#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>
#include <string>
#include <iostream>

static binadox::csp_header::nonce_info_t get_nonce_policy(const std::string& data)
{
    binadox::csp_header h = binadox::csp_header::parse(data.c_str(), data.c_str() + data.size());
    return h.get_nonce_policy();
}

TEST_CASE ("CSP scanner 1")
{
    std::string data = "default-src 'self' blob: https://*.cnn.com:* http://*.cnn.com:* *.cnn.io:* *.cnn.net:* *.turner.com:* *.turner.io:* *.ugdturner.com:* courageousstudio.com *.vgtf.net:*; script-src 'unsafe-eval' 'unsafe-inline' 'self' *; style-src 'unsafe-inline' 'self' blob: *; child-src 'self' blob: *; frame-src 'self' *; object-src 'self' *; img-src 'self' data: blob: *; media-src 'self' data: blob: *; font-src 'self' data: *; connect-src 'self' *; frame-ancestors 'self' https://*.cnn.com:* http://*.cnn.com https://*.cnn.io:* http://*.cnn.io:* *.turner.com:* courageousstudio.com;";

    binadox::csp_header h = binadox::csp_header::parse(data.c_str(), data.c_str() + data.size());
    std::string out = h.to_string();

    REQUIRE(out == data);
    REQUIRE(h.is_self_allowed());
}

TEST_CASE ("CSP scanner 2")
{
    std::string data = "default-src 'self'; script-src 'self' 'nonce-bdb5e024-0126-8e0d-da05-f6c4de4766d5' chrome-extension: 'unsafe-inline' 'unsafe-eval' *.canary.lwc.dev *.um5.visual.force.com https://ssl.gstatic.com/accessibility/ https://static.lightning.force.com https://*.salesforceliveagent.com/; object-src 'self' https://c.um5.content.force.com; style-src 'self' blob: chrome-extension: 'unsafe-inline' *.um5.visual.force.com https://c.um5.content.force.com; img-src 'self' http: https: data: blob: *.um5.visual.force.com; media-src 'self' *.um5.visual.force.com https://c.um5.content.force.com blob:; frame-ancestors 'self'; frame-src https: mailto: *.um5.visual.force.com; font-src 'self' https: data: *.um5.visual.force.com; connect-src 'self' https://api.bluetail.salesforce.com https://staging.bluetail.salesforce.com https://preprod.bluetail.salesforce.com blob: *.um5.visual.force.com https://c.um5.content.force.com https://um5.salesforce.com";

    binadox::csp_header h = binadox::csp_header::parse(data.c_str(), data.c_str() + data.size());
    std::string out = h.to_string();

    REQUIRE(out == data);
    REQUIRE(h.is_self_allowed());
}

TEST_SUITE("CSP - check nonce policy")
{
    TEST_CASE("gmail")
    {
        binadox::csp_header::nonce_info_t pol = get_nonce_policy("script-src 'nonce-Y4zlx0aR9W29iDCgfeCaMw' 'unsafe-inline' 'strict-dynamic' https: http: 'unsafe-eval';object-src 'none';base-uri 'self';report-uri https://mail.google.com/mail/cspreport");
        REQUIRE(pol.first == binadox::csp_header::ePRECALCULATED);
        REQUIRE(pol.second == "Y4zlx0aR9W29iDCgfeCaMw");
    }

    TEST_CASE("cnn")
    {
        binadox::csp_header::nonce_info_t pol = get_nonce_policy("default-src 'self' blob: https://*.cnn.com:* http://*.cnn.com:* *.cnn.io:* *.cnn.net:* *.turner.com:* *.turner.io:* *.ugdturner.com:* courageousstudio.com *.vgtf.net:*; script-src 'unsafe-eval' 'unsafe-inline' 'self' *; style-src 'unsafe-inline' 'self' blob: *; child-src 'self' blob: *; frame-src 'self' *; object-src 'self' *; img-src 'self' data: blob: *; media-src 'self' data: blob: *; font-src 'self' data: *; connect-src 'self' *; frame-ancestors 'self' https://*.cnn.com:* http://*.cnn.com https://*.cnn.io:* http://*.cnn.io:* *.turner.com:* courageousstudio.com;");
        REQUIRE(pol.first == binadox::csp_header::eSELF);
        REQUIRE(pol.second.empty());
    }

    TEST_CASE("ebay")
    {
        binadox::csp_header::nonce_info_t pol = get_nonce_policy("content-security-policy-report-only: default-src 'self' *.ebay.com *.ebaystatic.com; img-src 'self' data: *.ebay.com *.ebayimg.com *.ebayrtm.com *.ebaystatic.com; script-src 'self' 'unsafe-eval'  'nonce-3UykWY6k9MwjWiDL7OSWe0I0lsSLNO' *.ebay.com *.ebaystatic.com *.ebayrtm.com; style-src 'self' 'unsafe-inline' *.ebay.com *.ebaystatic.com;  report-uri /csp-report");
        REQUIRE(pol.first == binadox::csp_header::ePRECALCULATED);
        REQUIRE(pol.second == "3UykWY6k9MwjWiDL7OSWe0I0lsSLNO");
    }


}

#endif
