//
// Created by igor on 05/11/2020.
//

#include "plugin/content_encoding.hh"
#include "plugin/constants.hh"
#include "plugin/string_utils.hh"

namespace binadox
{
    std::string to_string(content_encoding_t enc)
    {
        switch (enc)
        {
            case eGZIP:
                return "gzip";
            case eDEFLATE:
                return "deflate";
            case eBR:
                return "br";
            case eNONE:
                return "none";
        }
        return "unknown";
    }
    // ---------------------------------------------------
    content_encoding_t encoding_from_string(const libecap::Area& header)
    {
        if (icase_contains(header, gzip))
        {
            return eGZIP;
        } else
        {
            if (icase_contains(header, deflate))
            {
                return binadox::eDEFLATE;
            } else
            {
                if (icase_contains(header, br))
                {
                    return binadox::eBR;
                } else
                {
                    return eNONE;
                }
            }
        }
    }
}
