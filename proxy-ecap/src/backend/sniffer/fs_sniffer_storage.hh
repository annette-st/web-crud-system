//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_FS_SNIFFER_STORAGE_HH
#define BINADOX_ECAP_FS_SNIFFER_STORAGE_HH
#include <sstream>
#include "backend/sniffer/sniffer_storage.hh"
namespace binadox
{
    class fs_sniffer_storage : public sniffer_storage
    {
    public:
        fs_sniffer_storage(const std::string& path);
        virtual ~fs_sniffer_storage();
    private:
        void enter_transaction() override;
        void exit_transaction() override;

        void write_request(const std::string& transaction_id,
                                   const std::string& method,
                                   const std::string& url,
                                   const std::string& client_ip,
                                   const headers_t& headers,
                                   uint64_t time_stamp,
                                   const std::string& script
        ) override;
        void write_response(const std::string& transaction_id,
                                    int http_code,
                                    const std::string& reason,
                                    const std::string& client_ip,
                                    const headers_t& headers,
                                    uint64_t time_stamp,
                                    const std::string& script
        ) override;
        void write_chunk(const std::string& transaction_id,
                                 bool is_request,
                                 int chunk_num,
                                 const std::vector<char>& chunk) override;
        void finalize(const std::string& transaction_id,
                              bool is_request) override;
    private:
        std::string get_transaction_path(const std::string& transaction_id);

        template <typename T>
        std::string get_transaction_path(const std::string& transaction_id, const T& val)
        {
            std::ostringstream os;
            os << get_transaction_path(transaction_id) << "/" << val;
            return os.str();
        }
    private:
        std::string root;
    };
} // ns binadox

#endif //BINADOX_ECAP_FS_SNIFFER_STORAGE_HH
