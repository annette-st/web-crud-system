//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_META_DATA_HH
#define BINADOX_ECAP_SNIFFER_META_DATA_HH

#include <map>
#include <string>
#include <cstdint>
#include <libecap/common/header.h>
#include <libecap/common/message.h>

#include "plugin/content_encoding.hh"

namespace binadox
{

    class request_info
    {
    public:
        request_info();
        request_info(const libecap::RequestLine* line);

        bool valid() const;
        const std::string& get_url() const;
        const std::string& get_method() const;

    private:
        std::string url;
        std::string method;
    };

    class response_info
    {
    public:
        response_info();
        response_info(const libecap::StatusLine* line);

        bool valid() const;
        int get_code() const;
        const std::string& get_reason() const;
    private:
        int code;
        std::string reason;
    };

    class sniffer_headers_visitor;
    class sniffer_meta_data
    {
        friend class sniffer_headers_visitor;
    public:
        typedef std::map<std::string, std::string> string_map_t;
    public:
        sniffer_meta_data(const libecap::RequestLine* line);
        sniffer_meta_data(const libecap::StatusLine*  line);

        void load_headers(libecap::shared_ptr<libecap::Message> msg);
        void set_client_ip(const std::string& ip);
        void set_script(const std::string& script_name);

        bool is_request() const;

        const response_info& get_response() const;
        const request_info& get_request() const;
        const std::string& get_client_ip() const;
        const std::string& get_transaction_id() const;
        void set_transaction_id(const std::string& x);
        const string_map_t& get_headers() const;
        const std::string get_content_type() const;
        content_encoding_t get_encoding () const;
        uint64_t get_timestamp() const;
        const std::string& get_script() const;

        std::string to_string() const;
    private:
        response_info response;
        request_info  request;
        std::string   ip;
        std::string   script;

        std::string   transaction_id;
        string_map_t  headers;

        content_encoding_t data_encoding;
        uint64_t creation_time;
        std::string content_type;
    };
} // ns binadox

#endif //BINADOX_ECAP_SNIFFER_META_DATA_HH
