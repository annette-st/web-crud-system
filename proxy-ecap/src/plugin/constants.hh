//
// Created by igor on 28/07/2020.
//

#ifndef BINADOX_ECAP_CONSTANTS_HH
#define BINADOX_ECAP_CONSTANTS_HH

#include <libecap/common/name.h>
#include <libecap/common/names.h>
#include <string>

namespace binadox
{
    extern const libecap::Name headerHost;
    extern const libecap::Name headerCacheControl;
    extern const std::string cacheControlMatch;
    extern const libecap::Name headerContentMD5;
    extern const libecap::Name headerContentType;
    extern const std::string text_html;
    extern const libecap::Name headerContentEncoding;
    extern const std::string gzip;
    extern const std::string deflate;
    extern const std::string br;
    extern const libecap::Name headerXclientip;
    extern const libecap::Name headerCSP;
    extern const libecap::Name headerCSPReport;
    extern const std::string unsafe_inline;
    extern const libecap::Name binadoxMark;

    extern const std::string acceptor_uuid;

    extern const libecap::Name transactionID;
    extern const libecap::Name originDomain;
    extern const libecap::Name addonOriginDomain;
    extern const libecap::Name accessControlAllowOrigin; // Access-Control-Allow-Origin
    extern const libecap::Name xFrameOptions; // X-FRAME-OPTIONS
    extern const libecap::Name xXSSProtection;//X-XSS-Protection

    extern const std::string audit_config;

    extern const std::string response_replace;
    extern const std::string response_replace_re;

    extern const std::string login_scripts_template;
    extern const std::string audit_config_template;
    extern const std::string onload_template;
}

#endif //BINADOX_ECAP_CONSTANTS_HH
