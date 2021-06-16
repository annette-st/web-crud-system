//
// Created by igor on 08/11/2020.
//

#include <sstream>
#include <nlohmann/json.hpp>

#include "plugin/sniffer_db/sniffer_ipc.hh"
#include "plugin/ipc/ipc.hh"

namespace binadox
{
    sniffer_ipc::sniffer_ipc(std::shared_ptr<ipc> ipc_impl)
    : pimpl(ipc_impl)
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::enter_transaction()
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::exit_transaction()
    {

    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::write_request(const std::string& transaction_id,
                                    const std::string& method,
                                    const std::string& url,
                                    const std::string& client_ip,
                                    const headers_t& headers,
                                    uint64_t time_stamp,
                                    const std::string& script
    )
    {
        nlohmann::json obj = nlohmann::json::object();
        obj["method"] = method;
        obj["url"] = url;
        obj["ip"] = client_ip;
        obj["time"] = time_stamp;
        if (!script.empty()) {
            obj["script"] = script;
        }
        nlohmann::json hdr = nlohmann::json::array();
        for (headers_t::const_iterator i = headers.begin(); i != headers.end(); i++)
        {
            nlohmann::json h = nlohmann::json::object();
            h["name"] = i->first;
            h["value"] = i->second;
            hdr.push_back(h);
        }
        obj["headers"] = hdr;
        std::ostringstream os;
        os << obj;
        auto msg = std::make_shared<std::string>(os.str());
        pimpl->start_transaction(transaction_id, true, msg);
    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::write_response(const std::string& transaction_id,
                                     int http_code,
                                     const std::string& reason,
                                     const std::string& client_ip,
                                     const headers_t& headers,
                                     uint64_t time_stamp,
                                     const std::string& script
    )
    {
        nlohmann::json obj = nlohmann::json::object();
        obj["code"] = http_code;
        obj["reason"] = reason;
        obj["ip"] = client_ip;
        obj["time"] = time_stamp;
        if (!script.empty()) {
            obj["script"] = script;
        }
        nlohmann::json hdr = nlohmann::json::array();
        for (headers_t::const_iterator i = headers.begin(); i != headers.end(); i++)
        {
            nlohmann::json h = nlohmann::json::object();
            h["name"] = i->first;
            h["value"] = i->second;
            hdr.push_back(h);
        }
        obj["headers"] = hdr;
        std::ostringstream os;
        os << obj;
        auto msg = std::make_shared<std::string>(os.str());
        pimpl->start_transaction(transaction_id, false, msg);
    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::write_chunk(const std::string& transaction_id,
                                  bool is_request,
                                  int chunk_num,
                                  const std::vector<char>& chunk)
    {
        pimpl->send_data(transaction_id, is_request, chunk_num, chunk);
    }
    // ------------------------------------------------------------------------------------------------------------
    void sniffer_ipc::finalize(const std::string& transaction_id,
                               bool is_request)
    {
        pimpl->end_transaction(transaction_id, is_request);
    }
}
