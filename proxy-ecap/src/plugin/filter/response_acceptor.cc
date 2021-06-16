//
// Created by igor on 28/07/2020.
//

#include "plugin/filter/response_acceptor.hh"
#include "plugin/constants.hh"
#include "plugin/string_utils.hh"
#include "plugin/content_encoding.hh"

#include <libecap/common/area.h>

namespace binadox
{
    response_acceptor::response_acceptor()
            : is_valid(true), content_type_seen(false), enc(binadox::eNONE),
              csp_header(0), csp_header_report_only(0)
    {

    }
    // -----------------------------------------------------------------------------------------------------
    response_acceptor::~response_acceptor()
    {
        delete csp_header_report_only;
        delete csp_header;
    }
    // -----------------------------------------------------------------------------------------------------
    bool response_acceptor::valid() const
    {
        return is_valid && content_type_seen;
    }
    // -----------------------------------------------------------------------------------------------------
    binadox::content_encoding_t response_acceptor::encoding() const
    {
        return enc;
    }
    // -----------------------------------------------------------------------------------------------------
    binadox::csp_header* response_acceptor::acquire_csp_header ()
    {
        binadox::csp_header* temp = csp_header;
        csp_header = 0;
        return temp;
    }
    // -----------------------------------------------------------------------------------------------------
    binadox::csp_header* response_acceptor::acquire_csp_header_report_only ()
    {
        binadox::csp_header* temp = csp_header_report_only;
        csp_header_report_only = 0;
        return temp;
    }
    // -----------------------------------------------------------------------------------------------------
    std::string response_acceptor::get_access_allow_origin () const
    {
        return access_allow_origin;
    }
    // -----------------------------------------------------------------------------------------------------
    std::string response_acceptor::get_xframe_options () const
    {
        return xframeoptions;
    }
    // -----------------------------------------------------------------------------------------------------
    std::string response_acceptor::get_xssprotection() const
    {
        return xxssprotection;
    }
    // -----------------------------------------------------------------------------------------------------
    void response_acceptor::visit(const libecap::Name& name, const libecap::Area& value)
    {
        if (iequals(name,binadoxMark))
        {
            is_valid = false;
        } else
        {
            if (!is_valid)
            {
                return;
            }
            if (iequals(name, headerCacheControl))
            {
               // if (icase_contains(value, cacheControlMatch))
              //  {
              //      is_valid = false;
              //      return;
              //  }
            } else
            {
                if (iequals(name, headerContentMD5))
                {
                    is_valid = false;
                    return;
                } else
                {
                    if (iequals(name, headerContentType))
                    {
                        content_type_seen = true;
                        if (!icase_contains(value, text_html))
                        {
                            is_valid = false;
                            return;
                        }
                    } else
                    {
                        if (iequals(name, headerContentEncoding))
                        {
                            enc = encoding_from_string(value);
                            if (enc == eNONE)
                            {
                                is_valid = false;
                                return;
                            }

                        } else {
                            if (iequals(name,headerCSP)) {
                                csp_header = new binadox::csp_header(binadox::csp_header::parse(value.start, value.start+value.size));
                            } else {
                                if (iequals(name,headerCSPReport)) {
                                    csp_header_report_only = new binadox::csp_header(binadox::csp_header::parse(value.start, value.start+value.size));
                                } else {
                                   if (iequals(name, accessControlAllowOrigin)) {
                                       access_allow_origin = value.toString();
                                   } else {
                                       if (iequals(name, xFrameOptions))
                                       {
                                           xframeoptions = name.image();
                                       } else {
                                           if (iequals(name, xXSSProtection))
                                           {
                                               xxssprotection = name.image();
                                           }
                                       }
                                   }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
