//
// Created by igor on 29/07/2020.
//

#ifndef BINADOX_ECAP_CSP_PARSER_HH
#define BINADOX_ECAP_CSP_PARSER_HH

#include "plugin/pipeline/csp/csp_header.hh"

namespace binadox
{
    struct script_gen_context
    {
        script_gen_context();
        bool valid;
        bool need_nonce;
        std::string nonce;
        bool has_unsafe_inline;
        bool has_frame_ancestors;
    };

    script_gen_context parse_csp(const csp_header* csp, const csp_header* csp_report, const std::string& default_nonce);

    void update_csp_header (const script_gen_context& ctx, csp_header* csp);

} // ns binadox

#endif //BINADOX_ECAP_CSP_PARSER_HH
