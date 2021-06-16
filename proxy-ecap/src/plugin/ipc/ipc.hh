//
// Created by igor on 04/08/2020.
//

#ifndef BINADOX_ECAP_IPC_HH
#define BINADOX_ECAP_IPC_HH

#include <string>
#include <vector>
#include <memory>

namespace binadox
{

    class failure_callable;

    class ipc
    {
        friend class failure_callable;
    public:
        enum message_type_t
        {
            eTRACKING_DATA,
            eLOGIN,
            ePERF_DATA,
            eKEY_VALUE
        };
    public:
        typedef std::shared_ptr<std::string> message_ptr_t;
    public:
        explicit ipc (ipc* fallback_ipc);
        virtual ~ipc();


        void start();
        void stop ();
        void send(message_type_t type, message_ptr_t message);

        // data transfer
        virtual void start_transaction(const std::string& trid, bool is_request, message_ptr_t message) = 0;
        virtual void end_transaction(const std::string& trid, bool is_request) = 0;
        virtual void send_data(const std::string& trid, bool is_request, int chunk_num, const std::vector<char>& chunk) = 0;
    protected:
        virtual bool on_start () = 0;
        virtual void on_stop  () = 0;
        virtual void on_send(message_type_t type, message_ptr_t message) = 0;

        void on_failure(message_type_t type, message_ptr_t message);
    private:
        ipc* fallback;
    };
    // -----------------------------------------------------------------------------
    class failure_callable
    {
    public:
        failure_callable(ipc* owner, ipc::message_type_t type, ipc::message_ptr_t message);
        void call();
    private:
        ipc* m_owner;
        ipc::message_type_t m_type;
        ipc::message_ptr_t m_message;
    };
} // ns ipc


#endif //BINADOX_ECAP_IPC_HH
