//
// Created by igor on 17/12/2020.
//

#include <sstream>
#include <cstring>
#include <algorithm>
#include "input_archive.hh"
#include <nlohmann/json.hpp>
#include "zip/ZipArchive.hpp"
#include "zip/ZipException.hpp"

namespace
{
    struct chunk_entry
    {
        int index;
        std::string entry;

        static chunk_entry create(const std::string ename)
        {
            chunk_entry r;
            r.entry = ename;
            r.index = -1;

            auto idx = ename.rfind(".");
            if (idx != std::string::npos)
            {
                std::istringstream is(ename.substr(idx + 1));
                is >> r.index;
            }
            return r;
        }
    };

    bool operator < (const chunk_entry& a, const chunk_entry& b)
    {
        return a.index < b.index;
    }

    class data_handler
    {
    public:
        data_handler() = default;
        virtual ~data_handler() = default;

        void handle(Zippy::ZipArchive& archive, const std::string& dir, std::vector<binadox::sniffer::chunk_t>& chunks)
        {
            auto all_names = archive.GetEntryNamesInDir(dir);
            std::vector<chunk_entry> order;

            for (const auto& entry : all_names)
            {
                auto idx = entry.rfind("/");
                if (idx != std::string::npos)
                {
                    auto entry_name = entry.substr(idx+1);

                    if (entry_name == "headers")
                    {
                        auto headers = archive.GetEntry(entry);

                        this->handle_headers(headers.GetDataAsString());
                    }
                    else
                    {
                        auto ce = chunk_entry::create(entry);
                        if (ce.index != -1)
                        {
                            order.push_back(ce);
                        }
                    }
                }
            }
            std::sort(order.begin(), order.end());

            for (const auto ce : order)
            {
                try
                {
                    auto c = archive.GetEntry(ce.entry);
                    chunks.push_back(c.GetData());
                } catch (std::exception&)
                {
                    chunks.push_back(binadox::sniffer::chunk_t());
                }

            }
        }

        static void parse_headers(binadox::sniffer::headers_t& hdr, const nlohmann::json& json)
        {
            for (const auto& obj : json["headers"])
            {
                hdr[obj["name"]] = obj["value"];
            }
        }

        virtual void handle_headers(const std::string& headers) = 0;
    };
    // ========================================================================================================
    class request_data_handler : public data_handler
    {
    public:
        request_data_handler(binadox::sniffer::transaction& tr)
        : m_tr(tr)
        {

        }

        void handle_headers(const std::string& headers_txt) override
        {
            auto jf = nlohmann::json::parse(headers_txt);
            m_tr.m_ip = jf["ip"].get<std::string>();
            m_tr.m_url = jf["url"].get<std::string>();

            m_tr.m_is_http = (m_tr.m_url.find("https://") == std::string::npos);

            auto idx = m_tr.m_url.find("//");
            auto idx2 = m_tr.m_url.find("/", idx+2);
            m_tr.m_domain = m_tr.m_url.substr(idx+2, idx2 - (idx+2));
            m_tr.m_resource = m_tr.m_url.substr(idx2+1);

            m_tr.m_request.m_method = jf["method"].get<std::string>();
            m_tr.m_request.m_time_stamp = jf["time"].get<uint64_t>();
            parse_headers(m_tr.m_request.m_headers, jf);
        }
    private:
        binadox::sniffer::transaction& m_tr;
    };
    // ========================================================================================================
    class resp_data_handler : public data_handler
    {
    public:
        resp_data_handler(binadox::sniffer::transaction& tr)
                : m_tr(tr)
        {

        }

        void handle_headers(const std::string& headers_txt) override
        {
            auto jf = nlohmann::json::parse(headers_txt);
            m_tr.m_ip = jf["ip"].get<std::string>();
            m_tr.m_response.m_code = jf["code"].get<int>();
            m_tr.m_response.m_time_stamp = jf["time"].get<uint64_t>();
            parse_headers(m_tr.m_response.m_headers, jf);
        }
    private:
        binadox::sniffer::transaction& m_tr;
    };
    // ========================================================================================================
    void handle_request(Zippy::ZipArchive& archive, const std::string& dir, binadox::sniffer::transaction& r)
    {
        request_data_handler dh(r);
        dh.handle(archive, dir, r.m_request.m_chunks);
    }

    void handle_response(Zippy::ZipArchive& archive, const std::string& dir, binadox::sniffer::transaction& r)
    {
        resp_data_handler dh(r);
        dh.handle(archive, dir, r.m_response.m_chunks);
    }
}

namespace binadox
{
    namespace sniffer
    {
        std::vector <transaction> load_transactions (const std::string& fname)
        {
            std::vector <transaction> result;

            Zippy::ZipArchive archive;
            archive.Open(fname);

            auto index = archive.GetEntry("index.txt");
            auto buf = index.GetDataAsString();

            std::string::size_type pos;
            while ((pos = buf.find ('\n')) != std::string::npos)
            {
                std::string trname = buf.substr (0, pos);
                auto all_names = archive.GetEntryNamesInDir(trname);
                if (!all_names.empty())
                {
                    transaction tr;
                    for (const auto& dir : all_names)
                    {
                        if (dir.find("/REQ/") != std::string::npos)
                        {
                            handle_request(archive, dir, tr);
                        }
                        else
                        {
                            handle_response(archive, dir, tr);
                        }
                    }
                    result.push_back(tr);
                }
                buf.erase (0, pos + 1);
            }

            return result;
        }
        // ================================================================================================
        transaction load_from_json(const std::string& json)
        {
            auto jf = nlohmann::json::parse(json);
            transaction tr;
            if (jf.contains("request"))
            {
                auto jreq = jf["request"];
                tr.m_url = jreq["url"].get<std::string>();
                tr.m_domain = jreq["host"].get<std::string>();
                tr.m_resource = jreq["path"].get<std::string>();

                tr.m_is_http = (jreq["protocol"].get<std::string>() == "HTTP");
                tr.m_ip = jreq["ip"];

                tr.m_request.m_method = jreq["method"].get<std::string>();
                tr.m_request.m_time_stamp = jreq["time"].get<uint64_t>();
                data_handler::parse_headers(tr.m_request.m_headers, jreq);
            }
            else
            {
                throw std::runtime_error("No request found");
            }
            if (jf.contains("response"))
            {
                auto jresp = jf["response"];
                tr.m_response.m_code = jresp["code"].get<int>();
                tr.m_response.m_time_stamp = jresp["time"].get<uint64_t>();
                data_handler::parse_headers(tr.m_response.m_headers, jresp);
                std::string body = jresp["body"].get<std::string>();
                chunk_t chunk(body.size());
                std::memcpy(chunk.data(), body.c_str(), body.size());
                tr.m_response.m_chunks.push_back(chunk);
            }
            return tr;
        }
    } // ns sniffer
} // ns binadox
