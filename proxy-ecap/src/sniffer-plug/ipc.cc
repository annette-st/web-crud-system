//
// Created by igor on 18/11/2020.
//


#include <iostream>
#include <sstream>
#include <cstring>
#include <termcolor/termcolor.hpp>
#include <nlohmann/json.hpp>

#include "ipc.hh"
#include "project.hh"

channel_iterator::channel_iterator (const std::string& channel)
: m_channel (channel), m_total(0), m_index(0)
{}
// ---------------------------------------------------------------------------------------------------
void channel_iterator::set_total(std::size_t total)
{
    m_total = total;
}
// ---------------------------------------------------------------------------------------------------
void channel_iterator::set_index(std::size_t index)
{
    m_index = index;
}
// ---------------------------------------------------------------------------------------------------
std::size_t channel_iterator::get_total() const
{
    return m_total;
}
// ---------------------------------------------------------------------------------------------------
std::size_t channel_iterator::get_index() const
{
    return m_index;
}
// ---------------------------------------------------------------------------------------------------
std::string channel_iterator::get_channel() const
{
    return m_channel;
}
// ---------------------------------------------------------------------------------------------------
void iterate_data_callback(redisAsyncContext* c, void* r, void* privdata)
{
    auto* reply = (redisReply*) r;
    bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
    if (failed)
    {
        std::cerr << termcolor::red << "ITERATION failed" << termcolor::reset << std::endl;
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        channel_iterator* itr = (channel_iterator*)privdata;
        itr->call(reply->str, reply->len);
        int idx = itr->get_index();
        idx++;
        if (idx >= itr->get_total())
        {
            delete itr;
        }
        else
        {
            itr->set_index(idx);
            redisAsyncCommand(c,
                              iterate_data_callback,
                              privdata,
                              "LINDEX %s %d", itr->get_channel().c_str(), idx);
        }
    }
}
// ---------------------------------------------------------------------------------------------------
void iterate_callback(redisAsyncContext* c, void* r, void* privdata)
{
    auto* reply = (redisReply*) r;
    bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
    if (failed)
    {
        std::cerr << termcolor::red << "ITERATION failed" << termcolor::reset << std::endl;
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        std::size_t total = reply->integer;
        channel_iterator* itr = (channel_iterator*)privdata;
        itr->set_total(total);
        itr->set_index(0);

        redisAsyncCommand(c,
                          iterate_data_callback,
                          privdata,
                          "LINDEX %s %d", itr->get_channel().c_str(), 0);
    }
}
// ---------------------------------------------------------------------------------------------------
void publish_callback(redisAsyncContext* c, void* r, void* privdata)
{
    auto* reply = (redisReply*) r;
    bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
    if (failed)
    {
        std::cerr << termcolor::red << "PUBLISH failed" << termcolor::reset << std::endl;
    }
}
// ---------------------------------------------------------------------------------------------------
void on_message_callback(redisAsyncContext* c, void* r, void* privdata)
{
    auto* reply = (redisReply*) r;
    if (!reply)
    {
        std::cerr << termcolor::red << "NULL reply" << termcolor::reset << std::endl;
        return;
    }
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 3)
    {
        if (reply->element[0]->type == REDIS_REPLY_STRING &&
            reply->element[0]->str &&
            strcmp(reply->element[0]->str, "message") == 0)
        {
            if (reply->element[2]->type == REDIS_REPLY_STRING && reply->element[2]->str)
            {
                const char* input = reply->element[2]->str;
                if (input)
                {
                    auto message = nlohmann::json::parse(input);
                    if (message.contains("type") && message["type"] == "sniffer")
                    {
                        if (message.contains("name"))
                        {
                            auto channel = message["name"];
                            project::instance().handle_data(channel);
                        }
                    }
                }
            } else
            {
                std::cerr << termcolor::red << "Unexpected reply sub format. Type:" << reply->element[2]->type;
            }
        }
    }
}
// ==================================================================================================
ipc::ipc(int port)
{
    conn.port = port;
    conn.host = "localhost";

    if (-1 == evthread_use_pthreads())
    {
        throw std::runtime_error("failed to initialize libevent pthreads");
    }

    publisher = std::unique_ptr<binadox::redis::publisher>(new binadox::redis::publisher("PROXY-RECV", conn));
    subscriber = std::unique_ptr<binadox::redis::subscriber>(new binadox::redis::subscriber("PROXY-IPC", conn));

    subscriber->attach(on_message_callback, nullptr);

}
// ---------------------------------------------------------------------
ipc::~ipc()
{

}
// ---------------------------------------------------------------------
void ipc::wait()
{
    publisher->wait();
    subscriber->wait();
}
// ---------------------------------------------------------------------
void ipc::stop()
{
    publisher->stop();
    subscriber->stop();
}
// ---------------------------------------------------------------------
void ipc::start_record(const std::string& ip)
{
    std::ostringstream os;
    os << R"({ "message" : "record-traffic", "from-ip" : ")"
    << (ip.empty() ? "127.0.0.1" : ip.c_str()) << "\"}";
    publisher->send(publish_callback, nullptr, os.str());
}
// ---------------------------------------------------------------------
void ipc::stop_record()
{
    publisher->send(publish_callback, nullptr, R"({ "message" : "record-traffic", "from-ip" : ""})");
}
// ---------------------------------------------------------------------
void ipc::iterate_channel(channel_iterator* visitor)
{
    publisher->llen(visitor->get_channel(), iterate_callback, visitor);
}