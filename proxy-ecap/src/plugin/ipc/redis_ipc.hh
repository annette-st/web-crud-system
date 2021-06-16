//
// Created by igor on 04/08/2020.
//

#ifndef BINADOX_ECAP_REDIS_IPC_HH
#define BINADOX_ECAP_REDIS_IPC_HH

#include "plugin/ipc/ipc.hh"
#include "backend/redis/redis_connection_parameters.hh"
#include "plugin/conf/configurator.hh"

#include <memory>


namespace binadox
{
    namespace redis
    {
        struct ipc_impl;

        class ipc : public binadox::ipc
        {
        public:
            ipc (binadox::ipc* fallback, const redis::connection_parameters& cp);
        private:
            virtual ~ipc();

            virtual bool on_start();
            virtual void on_stop ();
            virtual void on_send(message_type_t type, message_ptr_t message);

            void start_transaction(const std::string& trid, bool is_request, message_ptr_t message) override;
            void end_transaction(const std::string& trid, bool is_request) override;
            void send_data(const std::string& trid, bool is_request, int chunk_num, const std::vector<char>& chunk) override;
        private:
            ipc_impl* pimpl;

        };
    } // ns redis
} // ns binadox

#endif //BINADOX_ECAP_REDIS_IPC_HH
