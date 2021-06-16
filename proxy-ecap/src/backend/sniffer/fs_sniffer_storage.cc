//
// Created by igor on 23/08/2020.
//

#include <fstream>
#include "backend/sniffer/fs_sniffer_storage.hh"
#include "backend/work_dir.hh"
#include "backend/raise.hh"
#include <nlohmann/json.hpp>

namespace binadox
{
    fs_sniffer_storage::fs_sniffer_storage(const std::string& path)
            : root(path)
    {
        if (dir_exists(path))
        {
            if (!can_write(path))
            {
                RAISE(path, "is not writable");
            }
        } else
        {
            if (!mk_path(path))
            {
                RAISE("Can not create directory", path);
            }
        }
    }
    // --------------------------------------------------------------------------------------------
    fs_sniffer_storage::~fs_sniffer_storage()
    {

    }
    // --------------------------------------------------------------------------------------------
    void fs_sniffer_storage::enter_transaction()
    {

    }
    // --------------------------------------------------------------------------------------------
    void fs_sniffer_storage::exit_transaction()
    {

    }
    // --------------------------------------------------------------------------------------------
    std::string fs_sniffer_storage::get_transaction_path(const std::string& transaction_id)
    {
        std::ostringstream os;
        os << root << "/" << transaction_id;
        return os.str();
    }
    // --------------------------------------------------------------------------------------------
    void fs_sniffer_storage::write_request(const std::string& transaction_id,
                                           const std::string& method,
                                           const std::string& url,
                                           const std::string& client_ip,
                                           const headers_t& headers,
                                           uint64_t time_stamp,
                                           const std::string& script)
    {
        mk_path(get_transaction_path(transaction_id));
        nlohmann::json obj = nlohmann::json::object();
        obj["method"] = method;
        obj["url"] = url;
        obj["ip"] = client_ip;
        obj["time"] = time_stamp;
        if (!script.empty())
        {
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

        std::ofstream ofs(get_transaction_path(transaction_id, "request"));
        ofs << obj;
    }
    // --------------------------------------------------------------------------------------------
    void fs_sniffer_storage::write_response(const std::string& transaction_id,
                                            int http_code,
                                            const std::string& reason,
                                            const std::string& client_ip,
                                            const headers_t& headers,
                                            uint64_t time_stamp,
                                            const std::string& script)
    {
        mk_path(get_transaction_path(transaction_id));
        nlohmann::json obj = nlohmann::json::object();
        obj["code"] = http_code;
        obj["reason"] = reason;
        obj["ip"] = client_ip;
        obj["time"] = time_stamp;
        if (!script.empty())
        {
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

        std::ofstream ofs(get_transaction_path(transaction_id, "response"));
        ofs << obj;
    }
    // --------------------------------------------------------------------------------------------
    void fs_sniffer_storage::write_chunk(const std::string& transaction_id,
                                         bool is_request,
                                         int chunk_num,
                                         const std::vector<char>& chunk)
    {
        std::string path = get_transaction_path(transaction_id, chunk_num);
        if (is_request)
        {
            path += ".req";
        } else
        {
            path += ".resp";
        }
        std::ofstream ofs(path, std::ios::binary);
        ofs.write(chunk.data(), chunk.size());
    }
    // ---------------------------------------------------------------------------------------------
    void fs_sniffer_storage::finalize(const std::string& /*transaction_id*/,
                  bool /*is_request*/)
    {

    }
} // ns binadox
