//
// Created by igor on 25/07/2020.
//

#include "plugin/pipeline/pipeline_context.hh"
#include "plugin/constants.hh"
#include "plugin/string_utils.hh"

namespace binadox
{

    // ---------------------------------------------------
    pipeline_context::pipeline_context (content_encoding_t enc)
            : encoding(enc),
              content_security_policy(0),
              content_security_policy_report(0),
              sniffer_md(nullptr)
    {

    }
    // ---------------------------------------------------
    pipeline_context::pipeline_context (content_encoding_t enc, csp_header* csp, csp_header* csp_report)
    : encoding(enc),
    content_security_policy(csp),
    content_security_policy_report(csp_report),
    sniffer_md(nullptr)
    {

    }

} // ns binadox
