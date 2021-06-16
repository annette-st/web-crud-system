//
// Created by igor on 23/08/2020.
//

#include <sstream>

#include <libecap/common/area.h>
#include <libecap/common/name.h>
#include <libecap/common/named_values.h>

#include "plugin/sniffer_db/sniffer_meta_data.hh"
#include "plugin/constants.hh"
#include "plugin/string_utils.hh"

#include <nlohmann/json.hpp>

#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <time.h>

namespace
{
    uint64_t get_time_ms()
    {
        struct timespec spec;

        clock_gettime(CLOCK_REALTIME, &spec);
        return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
    }
}

namespace binadox
{
    class sniffer_headers_visitor : public libecap::NamedValueVisitor
    {
    public:
        sniffer_headers_visitor(sniffer_meta_data& owner)
        : data(owner)
        {

        }
    private:
        virtual void visit(const libecap::Name& name, const libecap::Area& value)
        {
            if (name == transactionID)
            {
                data.transaction_id = value.toString();
            } else
            {
                if (iequals(name, headerContentType))
                {
                    data.content_type = value.toString();
                    data.headers[name.image()] = data.content_type;
                } else
                {
                    if (iequals(name, headerContentEncoding))
                    {
                        data.data_encoding = encoding_from_string(value);
                    }
                    data.headers[name.image()] = value.toString();
                }

            }
        }
    private:
        sniffer_meta_data& data;
    };
    // =================================================================================
    request_info::request_info()
    {

    }
    // ---------------------------------------------------------------------------------
    request_info::request_info(const libecap::RequestLine* line)
    : url(line->uri().toString()),
      method(line->method().image())
    {

    }
    // ---------------------------------------------------------------------------------
    bool request_info::valid() const
    {
        return !method.empty();
    }
    // ---------------------------------------------------------------------------------
    const std::string& request_info::get_url() const
    {
        return url;
    }
    // ---------------------------------------------------------------------------------
    const std::string& request_info::get_method() const
    {
        return method;
    }
    // =================================================================================
    response_info::response_info()
    : code(-1)
    {

    }
    // ---------------------------------------------------------------------------------
    response_info::response_info(const libecap::StatusLine* line)
    : code(line->statusCode()),
    reason(line->reasonPhrase().toString())
    {

    }
    // ---------------------------------------------------------------------------------
    bool response_info::valid() const
    {
        return (code != -1);
    }
    // ---------------------------------------------------------------------------------
    int response_info::get_code() const
    {
        return code;
    }
    // ---------------------------------------------------------------------------------
    const std::string& response_info::get_reason() const
    {
        return reason;
    }
    // =================================================================================
    sniffer_meta_data::sniffer_meta_data(const libecap::RequestLine* line)
    : request(line),
      data_encoding(eNONE)
    {
        creation_time = get_time_ms();
    }
    // ----------------------------------------------------------------------------------
    sniffer_meta_data::sniffer_meta_data(const libecap::StatusLine*  line)
    : response(line),
      data_encoding(eNONE)
    {
        creation_time = get_time_ms();
    }
    // ----------------------------------------------------------------------------------
    void sniffer_meta_data::load_headers(libecap::shared_ptr<libecap::Message> msg)
    {
        sniffer_headers_visitor visitor(*this);
        msg->header().visitEach(visitor);
    }
    // ----------------------------------------------------------------------------------
    void sniffer_meta_data::set_client_ip(const std::string &client_ip)
    {
        ip = client_ip;
    }
    // ----------------------------------------------------------------------------------
    void sniffer_meta_data::set_script(const std::string& script_name)
    {
        script = script_name;
    }
    // ----------------------------------------------------------------------------------
    bool sniffer_meta_data::is_request() const
    {
        return request.valid();
    }
    // ----------------------------------------------------------------------------------
    const response_info& sniffer_meta_data::get_response() const
    {
        return response;
    }
    // ----------------------------------------------------------------------------------
    const request_info& sniffer_meta_data::get_request() const
    {
        return request;
    }
    // ----------------------------------------------------------------------------------
    const std::string& sniffer_meta_data::get_client_ip() const
    {
        return ip;
    }
    // ----------------------------------------------------------------------------------
    const std::string& sniffer_meta_data::get_transaction_id() const
    {
        return transaction_id;
    }
    // ----------------------------------------------------------------------------------
    void sniffer_meta_data::set_transaction_id(const std::string& x)
    {
        transaction_id = x;
    }
    // ----------------------------------------------------------------------------------
    const sniffer_meta_data::string_map_t& sniffer_meta_data::get_headers() const
    {
        return headers;
    }
    // ----------------------------------------------------------------------------------
    uint64_t sniffer_meta_data::get_timestamp() const
    {
        return creation_time;
    }
    // ----------------------------------------------------------------------------------
    const std::string& sniffer_meta_data::get_script() const
    {
        return script;
    }
    // ----------------------------------------------------------------------------------
    content_encoding_t sniffer_meta_data::get_encoding () const
    {
        return data_encoding;
    }
    // ----------------------------------------------------------------------------------
    const std::string sniffer_meta_data::get_content_type() const
    {
        return content_type;
    }
    // ----------------------------------------------------------------------------------
    std::string sniffer_meta_data::to_string() const
    {
        nlohmann::json obj = nlohmann::json::object();

        obj["ip"] = ip;
        if (!script.empty()) {
            obj["script"] = script;
        }
        obj["transaction_id"] = transaction_id;
        obj["time"] = creation_time;
        obj["content-type"] = content_type;
        nlohmann::json hdr = nlohmann::json::array();
        for (string_map_t::const_iterator i = headers.begin(); i != headers.end(); i++)
        {
            nlohmann::json h = nlohmann::json::object();
            h["name"] = i->first;
            h["value"] = i->second;
            hdr.push_back(h);
        }
        obj["headers"] = hdr;
        obj ["request"] = is_request();
        if (is_request())
        {
            nlohmann::json payload = nlohmann::json::object();
            payload["url"] = get_request().get_url();
            payload["method"] = get_request().get_method();
            obj["payload"] = payload;
        }
        else
        {
            nlohmann::json payload = nlohmann::json::object();
            payload["code"] = get_response().get_code();
            payload["reason"] = get_response().get_reason();
            obj["payload"] = payload;
        }
        std::ostringstream os;
        os << obj;
        return os.str();
    }
} // ns binadox
