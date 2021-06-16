//
// Created by igor on 04/08/2020.
//

#include <thread>
#include <mutex>
#include <unistd.h>
#include <event2/event.h>

#include <hiredis/hiredis.h>
#include <hiredis/async.h>

#include "plugin/ipc/redis_ipc.hh"
#include "plugin/ipc/message_parser.hh"
#include "backend/redis/redis_io.hh"

#include "plugin/processors/perf_counter.hh"
#include "backend/unused.hh"
#include "backend/ev_util.hh"
#include "plugin/logger.hh"

#include <nlohmann/json.hpp>
/*
 * PUBLISH PROXY-RECV '{"message" : "record-traffic", "from-ip" : "127.0.0.1" }'
 */

static const int REQUEST_CONFIG_TIMEOUT_SEC = 10;
static const int PERFDATA_TIMEOUT_SEC = 30*60;
namespace binadox
{
    namespace redis
    {
        static void publish_callback(redisAsyncContext* c, void* reply, void* privdata);
        static void get_configs_callback(redisAsyncContext* c, void* reply, void* privdata);
        static void send_perf_data_callback(evutil_socket_t fd, short what, void* arg);
        static void on_message_callback(redisAsyncContext* c, void* reply, void* privdata);

        struct ipc_impl
        {
            explicit ipc_impl(const redis::connection_parameters& cp);
            ~ipc_impl();

            // -------------------------------------------------------------------------------------------
            void stop();
            void start_redis();
            void handle_incoming_message(const char* input);


            // -------------------------------------------------------------------------------------------
            connection_parameters conn;
            event_base* event_loop;
            event* perf_data_event;
            event* request_configs_event;

            std::unique_ptr<redis::publisher> publisher;
            std::unique_ptr<redis::subscriber> subscriber;

            ev_loop_thread runner;
            bool connected;

            std::mutex m_sub_mtx;
            std::condition_variable m_sub_cond;
            bool m_subscribed;
        };

        ipc_impl::ipc_impl(const redis::connection_parameters& cp)
                : conn(cp),
                  perf_data_event(nullptr),
                  request_configs_event(nullptr),
                  connected(false),
                  m_subscribed(false)
        {
            event_loop = runner.start();
        }
        // --------------------------------------------------------------------------------------------
        static void send_get_configs_command(UNUSED_ARG evutil_socket_t fd, UNUSED_ARG short what, void* privdata)
        {
            auto* impl = (ipc_impl*)privdata;
            static bool first_time = true;
            if (first_time)
            {
                LOG_TRACE("Request for configs");
                first_time = false;
            }
            impl->publisher->send(get_configs_callback, impl, R"({ "type" : "get-configs"})");
        }
        // -------------------------------------------------------------------------------------------
        ipc_impl::~ipc_impl()
        {
            stop();
        }
        // -------------------------------------------------------------------------------------------
        void ipc_impl::stop()
        {
            if (perf_data_event)
            {
                event_free(perf_data_event);
            }
            if (request_configs_event)
            {
                event_free(request_configs_event);
            }
            subscriber->stop();
            publisher->stop();

            connected = false;
        }
        // -------------------------------------------------------------------------------------------
        void ipc_impl::start_redis()
        {
            publisher = std::unique_ptr<redis::publisher>(new redis::publisher("PROXY-IPC", conn));
            subscriber = std::unique_ptr<redis::subscriber>(new redis::subscriber("PROXY-RECV",conn));
            LOG_TRACE("Redis publisher and subscriber stared");
        }
        // -------------------------------------------------------------------------------------------
        void ipc_impl::handle_incoming_message(const char* input)
        {
            try
            {
                parse_ipc_message(input);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("Error handling message : ", e.what());
            }
            catch (...)
            {
                LOG_ERROR("Error handling message");
            }
        }
        // ----------------------------------------------------------------------------------
        void publish_callback(UNUSED_ARG redisAsyncContext* c, void* r, void* privdata)
        {
            auto* reply = (redisReply*) r;
            auto* cbk = (failure_callable*) privdata;

            bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
            if (reply->type == REDIS_REPLY_INTEGER)
            {
                const int num_receivers = reply->integer;
                if (num_receivers == 0)
                {
                    failed = true;
                }
            }
            if (failed)
            {
                cbk->call();
            }
            delete cbk;
        }
        // ----------------------------------------------------------------------------------
        void get_configs_callback(UNUSED_ARG redisAsyncContext* c, void* r, void* privdata)
        {
            auto* reply = (redisReply*) r;
            auto* impl = (ipc_impl*)privdata;

            bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
            if (failed)
            {
                LOG_ERROR("Failed to get configs, retry");
            } else {
                if (reply->type == REDIS_REPLY_INTEGER)
                {
                    const int num_receivers = reply->integer;
                    if (num_receivers == 0)
                    {
                        LOG_ERROR("Proxy backend is not connected, retrying in ", REQUEST_CONFIG_TIMEOUT_SEC, "secs");

                        if (!impl->request_configs_event)
                        {
                            struct timeval timeout = {REQUEST_CONFIG_TIMEOUT_SEC, 0};
                            impl->request_configs_event = event_new(impl->event_loop, -1, EV_PERSIST, send_get_configs_command, impl);

                            int rc = event_add(impl->request_configs_event, &timeout);

                            LOG_TRACE("Config rc ", rc);
                        }
                    }
                    else
                    {
                        if (impl->request_configs_event)
                        {
                            LOG_TRACE("Disabling periodic config requests");
                            event_del(impl->request_configs_event);
                        }
                        LOG_TRACE("Proxy backend is connected, waiting for configs");
                    }
                }
            }
        }
        // ----------------------------------------------------------------------------------
        void send_perf_data_callback(UNUSED_ARG evutil_socket_t fd, UNUSED_ARG short what, void* arg)
        {
            auto* obj = (redis::ipc*) arg;
            if (get_config())
            {
                perf_counter_data dt;
                get_config()->get_perf_counter().swap(dt);

                if (!dt.empty())
                {
                    std::ostringstream os;
                    dt.serialize(os);
                    obj->send(ipc::ePERF_DATA, std::make_shared<std::string>(os.str()));
                }
            }
        }
        // -----------------------------------------------------------------------------
        void on_message_callback(UNUSED_ARG redisAsyncContext* c, void* r, void* privdata)
        {
            auto* reply = (redisReply*) r;
            if (!reply)
            {
                LOG_ERROR("NULL reply");
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
                        auto* obj = (ipc_impl*) privdata;
                        obj->handle_incoming_message(reply->element[2]->str);
                    } else
                    {
                        LOG_ERROR("Unexpected reply sub format. Type:", reply->element[2]->type,
                                  " STR:", reply->element[2]->str == nullptr);
                    }
                } else
                {
                    const char* s = !reply->element[0]->str ? "NULL" : reply->element[0]->str;

                    if (reply->type == REDIS_REPLY_ARRAY && 0 == strcmp(s, "subscribe"))
                    {
                        if (reply->elements > 1)
                        {
                            LOG_ERROR("Subscribed to redis channel", reply->element[1]->str);

                            auto* obj = (ipc_impl*) privdata;
                            std::lock_guard<std::mutex> g(obj->m_sub_mtx);
                            obj->m_subscribed = true;
                            obj->m_sub_cond.notify_all();
                        }
                    }
                    else
                    {
                        if (reply->type == REDIS_REPLY_ERROR)
                        {
                            const char* err = !reply->str ? "Unknown error" : reply->str;
                            LOG_ERROR("Redis error: ", err);
                        }
                        else
                        {
                            LOG_ERROR("Unexpected reply format. Type:", reply->type,
                                      "ELTYPE: ", reply->element[0]->type,
                                      " STR:", s);
                        }
                    }
                }
            }
        }
        // ==================================================================================
        ipc::ipc(binadox::ipc* fallback, const connection_parameters& cp)
                : binadox::ipc(fallback)
        {
            pimpl = new ipc_impl(cp);
        }
        // ----------------------------------------------------------------------------------
        ipc::~ipc()
        {
            delete pimpl;
        }
        // ----------------------------------------------------------------------------------
        bool ipc::on_start()
        {
            pimpl->start_redis();

            bool result = pimpl->publisher->auth() && pimpl->subscriber->auth();
            pimpl->connected = result;

            if (!result)
            {
                LOG_ERROR("Can not connect to REDIS");
                pimpl->stop();
            }
            else
            {
                LOG_TRACE("Connected to REDIS");
                LOG_TRACE("Starting IPC, perf_data timeout is ", PERFDATA_TIMEOUT_SEC, "seconds");

                struct timeval timeout = {PERFDATA_TIMEOUT_SEC, 0};
                //struct timeval timeout = {5, 0};
                pimpl->perf_data_event = event_new(pimpl->event_loop, -1, EV_PERSIST, send_perf_data_callback, this);
                event_add(pimpl->perf_data_event, &timeout);
                pimpl->subscriber->attach(on_message_callback, pimpl);


                std::unique_lock<std::mutex> mlock(pimpl->m_sub_mtx);
                pimpl->m_sub_cond.wait(mlock, std::bind(&ipc_impl::m_subscribed, pimpl));
                if (get_config()->is_dynamic_config())
                {
                    send_get_configs_command(-1, -1, pimpl);
                }
            }
            return result;
        }
        // ----------------------------------------------------------------------------------
        void ipc::on_stop()
        {
            if (pimpl->connected)
            {
                pimpl->publisher->disconnect();
                pimpl->subscriber->disconnect();
            }
            pimpl->stop();
        }
        // ----------------------------------------------------------------------------------
        void ipc::on_send(message_type_t type, message_ptr_t message)
        {
            if (pimpl->publisher->is_connected())
            {
                LOG_TRACE("PUBLISH PROXY-IPC '", message->c_str(), "'");
                pimpl->publisher->send(publish_callback, new failure_callable(this, type, message), *message);
            }
        }
        // ---------------------------------------------------------------------------------
        static std::string get_channel_name(const std::string& trid, bool is_request)
        {
            std::ostringstream os;
            os << (is_request ? "REQ-" : "RESP-") << trid;
            return os.str();
        }
        // ---------------------------------------------------------------------------------
        void ipc::start_transaction(const std::string& trid, bool is_request, message_ptr_t message)
        {
            if (pimpl->publisher->is_connected())
            {
                pimpl->publisher->rpush(get_channel_name(trid, is_request), *message);
            }
        }
        // ---------------------------------------------------------------------------------
        void ipc::end_transaction(const std::string& trid, bool is_request)
        {
            if (pimpl->publisher->is_connected())
            {
                auto channel_name = get_channel_name(trid, is_request);
                LOG_TRACE("PUBLISH PROXY-IPC transaction '", channel_name, "'");
                std::ostringstream os;
                os << R"({ "type" : "sniffer", "name" : ")" << channel_name << R"("})";
                pimpl->publisher->send(os.str());
            }
        }
        // ---------------------------------------------------------------------------------
        void ipc::send_data(const std::string& trid, bool is_request, int chunk_num, const std::vector<char>& chunk)
        {
            if (pimpl->publisher->is_connected())
            {
                std::ostringstream os;
                os << R"({ "type" : "chunk", "id" : ")" << chunk_num << R"("})";
                pimpl->publisher->rpush(get_channel_name(trid, is_request), os.str(), chunk);
            }
        }
    } // ns redis
} // ns binadox

