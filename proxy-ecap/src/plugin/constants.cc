//
// Created by igor on 28/07/2020.
//

#include "plugin/constants.hh"

namespace binadox
{
    const libecap::Name headerHost("Host");
    const libecap::Name headerCacheControl("Cache-Control");
    const std::string cacheControlMatch("no-transform");
    const libecap::Name headerContentMD5("Content-MD5");
    const libecap::Name headerContentType("Content-Type");
    const std::string text_html("text/html");
    const libecap::Name headerContentEncoding("Content-Encoding");
    const std::string gzip("gzip");
    const std::string deflate("deflate");
    const std::string br("br");
    const libecap::Name headerXclientip("X-Client-IP");
    const libecap::Name headerCSP("Content-Security-Policy");
    const libecap::Name headerCSPReport("Content-Security-Policy-Report-Only");
    const std::string unsafe_inline("'unsafe-inline'");

    const std::string acceptor_uuid("3f785034-d25a-11ea-87d0-0242ac130003");
    const std::string audit_config("audit_config");
    const std::string response_replace("response_replace");
    const std::string response_replace_re("response_replace_re");
    const std::string login_scripts_template("LOGIN_SCRIPT");
    const std::string audit_config_template("AUDIT_CONFIG");
    const std::string onload_template("ONLOAD_SCRIPT");

    const libecap::Name transactionID("X-BINADOX-TR-ID");
    const libecap::Name originDomain("X-BINADOX-ORIGIN-DOMAIN");
    const libecap::Name addonOriginDomain("X-BINADOX-ORIGIN");
    const libecap::Name accessControlAllowOrigin("Access-Control-Allow-Origin");
    const libecap::Name xFrameOptions("X-FRAME-OPTIONS");
    const libecap::Name xXSSProtection("X-XSS-Protection");
    const libecap::Name binadoxMark("X-Binadox-Request");


} // ns binadox
