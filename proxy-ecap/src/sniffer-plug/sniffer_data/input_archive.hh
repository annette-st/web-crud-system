//
// Created by igor on 17/12/2020.
//

#ifndef BINADOX_ECAP_INPUT_ARCHIVE_HH
#define BINADOX_ECAP_INPUT_ARCHIVE_HH

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace binadox
{
    namespace sniffer
    {
        using chunk_t = std::vector<unsigned char>;
        using headers_t = std::map<std::string, std::string>;

        struct unit
        {
            uint64_t             m_time_stamp;
            headers_t            m_headers;
            std::vector<chunk_t> m_chunks;
        };

        struct request : public unit
        {
            std::string m_method;
        };

        struct response : public unit
        {
            int m_code;
        };

        struct transaction
        {
            std::string m_url;
            std::string m_domain;
            std::string m_resource;
            bool m_is_http;
            std::string m_ip;
            request m_request;
            response m_response;
        };

        std::vector <transaction> load_transactions (const std::string& fname);

        transaction load_from_json(const std::string& json);
    } // ns sniffer
}

#endif //BINADOX_ECAP_INPUT_ARCHIVE_HH
