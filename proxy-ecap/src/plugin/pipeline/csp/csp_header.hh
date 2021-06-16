//
// Created by igor on 20/07/2020.
//

#ifndef BINADOX_ECAP_CSP_HEADER_HH
#define BINADOX_ECAP_CSP_HEADER_HH

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace binadox
{
    namespace detail
    {
        class builder;
    }
    class csp_header
    {
        friend class detail::builder;
    public:
        static const std::string SCRIPT_SRC;
        static const std::string DEFAULT_SRC;
        static const std::string FRAME_ANCSETORS;//frame-ancestors;
    public:
        enum nonce_policy_t
        {
            eNONE,
            eSHA,
            ePRECALCULATED,
            eSELF,
            eDISABLED
        };
    public:
        typedef std::pair<nonce_policy_t, std::string> nonce_info_t;
    public:
        static csp_header parse(const char* header_begin, const char* header_end);
        std::string to_string() const;

        nonce_info_t get_nonce_policy() const;
        bool is_self_allowed() const;
        bool has_keyword (const std::string& section, const std::string& keyword) const;
        bool has_section (const std::string& section) const;
        void add_nonce(const std::string& value);
        void add_self();
        void remove_frame_ancestors();
    private:
        bool get_nonce_policy(const std::string& section, nonce_policy_t& policy, std::string& value) const;

    private:
        csp_header();

        struct directive_entry
        {
            directive_entry (const std::string& avalue)
            : value(avalue)
            {

            }
            std::string value;
        };
        typedef std::vector<directive_entry> entries_t;
        struct directive
        {
            entries_t entries;
        };
        typedef std::map<std::string, directive> directives_t;

        directives_t directives;
        typedef std::vector<std::string> string_vec_t;
        string_vec_t order;
        bool has_last_semi;
    private:
        static std::string NONCE;
        static std::string SHA256;
        static std::string SELF;
        static std::string NONE;
    };
} // ns binadox



#endif //BINADOX_ECAP_CSP_HEADER_HH
