//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_STORAGE_HH
#define BINADOX_ECAP_SNIFFER_STORAGE_HH

#include <string>
#include <map>
#include <vector>
#include <time.h>

namespace binadox
{
    class sniffer_storage
    {
    public:
        typedef std::map<std::string, std::string> headers_t;
    public:
        sniffer_storage();
        virtual ~sniffer_storage();

        virtual void enter_transaction() = 0;
        virtual void exit_transaction() = 0;

        virtual void write_request(const std::string& transaction_id,
                                   const std::string& method,
                                   const std::string& url,
                                   const std::string& client_ip,
                                   const headers_t& headers,
                                   uint64_t time_stamp,
                                   const std::string& script
                                   ) = 0;
        virtual void write_response(const std::string& transaction_id,
                                   int http_code,
                                   const std::string& reason,
                                   const std::string& client_ip,
                                   const headers_t& headers,
                                   uint64_t time_stamp,
                                    const std::string& script
                                   ) = 0;
        virtual void write_chunk(const std::string& transaction_id,
                                 bool is_request,
                                 int chunk_num,
                                 const std::vector<char>& chunk) = 0;
        virtual void finalize(const std::string& transaction_id,
                              bool is_request) = 0;

    };
} // ns binadox

#endif //BINADOX_ECAP_SNIFFER_STORAGE_HH
