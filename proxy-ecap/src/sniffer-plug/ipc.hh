//
// Created by igor on 18/11/2020.
//

#ifndef BINADOX_ECAP_IPC_HH
#define BINADOX_ECAP_IPC_HH

#include <string>
#include <thread>
#include <mutex>

#include <event2/event.h>
#include <event2/thread.h>

#include <hiredis/hiredis.h>
#include <hiredis/async.h>

#include "backend/redis/redis_io.hh"
// ==========================================================================
class channel_iterator
{
public:
    explicit channel_iterator (const std::string& channel);
    virtual ~channel_iterator() = default;

    virtual void call (const char* data, std::size_t length) = 0;

    void set_total(std::size_t total);
    void set_index(std::size_t index);

    std::size_t get_total() const;
    std::size_t get_index() const;

    std::string get_channel() const;

private:
    std::string m_channel;
    std::size_t m_total;
    std::size_t m_index;
};
// ==========================================================================
class ipc
{
public:
    ipc(int port);
    ~ipc();

    void start_record(const std::string& ip);
    void stop_record();

    void iterate_channel(channel_iterator* visitor);

    void wait();
    void stop();

private:
    binadox::redis::connection_parameters conn;

    std::unique_ptr<binadox::redis::publisher> publisher;
    std::unique_ptr<binadox::redis::subscriber> subscriber;

    bool connected;
};


#endif //BINADOX_ECAP_IPC_HH
