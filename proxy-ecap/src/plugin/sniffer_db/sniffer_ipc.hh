//
// Created by igor on 08/11/2020.
//

#ifndef BINADOX_ECAP_SNIFFER_IPC_HH
#define BINADOX_ECAP_SNIFFER_IPC_HH

#include <cstdint>
#include "backend/sniffer/sniffer_storage.hh"

namespace binadox
{
    class ipc;

    class sniffer_ipc : public sniffer_storage
    {
    public:
        explicit sniffer_ipc(std::shared_ptr<ipc> ipc_impl);

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
        std::shared_ptr<ipc> pimpl;
    };
}

#endif //BINADOX_ECAP_SNIFFER_IPC_HH
