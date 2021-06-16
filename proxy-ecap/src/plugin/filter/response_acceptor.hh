//
// Created by igor on 28/07/2020.
//

#ifndef BINADOX_ECAP_RESPONSE_ACCEPTOR_HH
#define BINADOX_ECAP_RESPONSE_ACCEPTOR_HH

#include <string>
#include <libecap/common/named_values.h>
#include "plugin/pipeline/pipeline_context.hh"


namespace binadox
{
    class response_acceptor : public libecap::NamedValueVisitor
    {
    public:
        response_acceptor();
        ~response_acceptor();

        bool valid() const;

        binadox::content_encoding_t encoding() const;
        binadox::csp_header* acquire_csp_header ();
        binadox::csp_header* acquire_csp_header_report_only ();

        std::string get_access_allow_origin () const;
        std::string get_xframe_options () const;
        std::string get_xssprotection() const;
    private:
        void visit(const libecap::Name& name, const libecap::Area& value);
    private:
        bool is_valid;
        bool content_type_seen;
        binadox::content_encoding_t enc;
        binadox::csp_header* csp_header;
        binadox::csp_header* csp_header_report_only;
        std::string access_allow_origin;
        std::string xframeoptions;
        std::string xxssprotection;

    };
} // ns binadox

#endif //BINADOX_ECAP_RESPONSE_ACCEPTOR_HH
