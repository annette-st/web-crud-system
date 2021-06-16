//
// Created by igor on 04/08/2020.
//

#include "plugin/ipc/ipc.hh"

namespace binadox
{
    ipc::ipc (ipc* fallback_ipc)
    : fallback(fallback_ipc)
    {

    }
    // -------------------------------------------------------------------
    ipc::~ipc()
    {
        delete fallback;
    }
    // -------------------------------------------------------------------
    void ipc::start()
    {
        on_start();
        if (fallback)
        {
            fallback->start();
        }
    }
    // -------------------------------------------------------------------
    void ipc::stop ()
    {
        on_stop();
        if (fallback)
        {
            fallback->stop();
        }
    }
    // -------------------------------------------------------------------
    void ipc::send(message_type_t type, message_ptr_t message)
    {
        on_send (type, message);
    }
    // -------------------------------------------------------------------
    void ipc::on_failure(message_type_t type, message_ptr_t message)
    {
        if (fallback)
        {
            fallback->send(type, message);
        }
    }
    // ================================================================================================
    failure_callable::failure_callable(ipc* owner, ipc::message_type_t type, ipc::message_ptr_t message)
    : m_owner(owner),
    m_type(type),
    m_message(message)
    {

    }
    // -------------------------------------------------------------------------------------------------
    void failure_callable::call()
    {
        m_owner->on_failure(m_type, m_message);
    }
} // ns binadox
