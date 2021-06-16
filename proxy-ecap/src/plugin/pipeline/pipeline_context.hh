//
// Created by igor on 25/07/2020.
//

#ifndef BINADOX_ECAP_PIPELINE_CONTEXT_HH
#define BINADOX_ECAP_PIPELINE_CONTEXT_HH

#include "plugin/pipeline/csp/csp_header.hh"
#include "plugin/conf/configurator.hh"
#include "plugin/content_encoding.hh"

namespace binadox
{
    class sniffer_meta_data;

    struct pipeline_context
    {
        explicit pipeline_context (content_encoding_t enc);
        pipeline_context (content_encoding_t enc, csp_header* csp, csp_header* csp_report);

        content_encoding_t encoding;
        std::string origin_ip;
        std::string destination_url;
        csp_header* content_security_policy;
        csp_header* content_security_policy_report;
        sniffer_meta_data* sniffer_md;

        std::string access_allow_origin;
        std::string addon_origin;
        std::string xframeoptions;
        std::string xxssprotection;
    };
} // ns binadox

#endif //BINADOX_ECAP_PIPELINE_CONTEXT_HH
