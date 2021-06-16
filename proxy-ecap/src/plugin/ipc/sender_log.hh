//
// Created by igor on 06/08/2020.
//

#ifndef BINADOX_ECAP_SENDER_LOG_HH
#define BINADOX_ECAP_SENDER_LOG_HH

#include <string>
#include "plugin/ipc/ipc.hh"
#include "backend/fs.hh"

namespace binadox
{
    struct sender_log_impl;



    class sender_log : public ipc
    {
    public:
        sender_log(const fs::path& path);
        ~sender_log();

        class visitor
        {
        public:
            virtual ~visitor();
            virtual void visit(int timestamp, message_type_t type, const std::string& message) = 0;
        };

        void empty_log(visitor& v);
    private:
        bool on_start () override;
        void on_stop  () override;
        void on_send  (message_type_t type, message_ptr_t message) override;

        void start_transaction(const std::string& trid, bool is_request, message_ptr_t message) override;
        void end_transaction(const std::string& trid, bool is_request) override;
        void send_data(const std::string& trid, bool is_request, int chunk_num, const std::vector<char>& chunk) override;
    private:
        sender_log_impl* pimpl;
    };
}

#endif //BINADOX_ECAP_SENDER_LOG_HH
