//
// Created by igor on 29/09/2020.
//

#include <sstream>
#include <mutex>

#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

#include "redis_io.hh"
#include "backend/unused.hh"

namespace binadox
{
    namespace redis
    {
        io::~io() = default;
        // ========================================================================================
        subscriber::subscriber(const std::string& channel, const connection_parameters& cp)
        : m_name(channel),
          conn (cp),
          redis_context(nullptr),
          connected(false)
        {

            event_loop = runner.start();

            redis_context = redisAsyncConnect(cp.host.c_str(), cp.port);
            if (redis_context->err)
            {
                std::ostringstream os;
                os << "Failed to connect to REDIS : ";
                if (redis_context->errstr)
                {
                    os << redis_context->errstr;
                } else
                {
                    os << redis_context->err;
                }
                throw std::runtime_error(os.str());
            }

            redisLibeventAttach(redis_context, event_loop);

            redisAsyncSetConnectCallback(redis_context, connect_callback);
            redisAsyncSetDisconnectCallback(redis_context, disconnect_callback);

            redis_context->data = this;
        }
        // -------------------------------------------------------------------------------
        void subscriber::attach(redisCallbackFn *fn, void *privdata)
        {
            redisAsyncCommand(redis_context,
                              fn,
                              privdata,
                              "SUBSCRIBE %s", m_name.c_str());
        }
        // -------------------------------------------------------------------------------
        void subscriber::disconnect()
        {
            if (connected && redis_context)
            {
                redisAsyncDisconnect(redis_context);
            }
        }
        // -------------------------------------------------------------------------------
        subscriber::~subscriber()
        {
            disconnect();
            if (redis_context)
            {
                redisAsyncFree(redis_context);
                redis_context = nullptr;
            }
        }
        // -------------------------------------------------------------------------------
        bool subscriber::auth()
        {
            auto connf = conn_promise.get_future();
            bool result = connf.get();
            if (result)
            {
                if (!conn.password.empty())
                {
                    if (conn.username.empty())
                    {
                        redisAsyncCommand(redis_context, auth_callback, nullptr, "AUTH %s",
                                          conn.password.c_str());
                    } else
                    {
                        redisAsyncCommand(redis_context, auth_callback, nullptr, "AUTH %s %s",
                                          conn.username.c_str(), conn.password.c_str());
                    }

                    std::future<bool> authf = auth_promise.get_future();
                    result = authf.get();
                }
            }
            return result;
        }
        // -------------------------------------------------------------------------------
        void subscriber::connect_callback(const redisAsyncContext *c, int status)
        {
            auto* obj = (subscriber*)c->data;
            obj->connected = status == REDIS_OK;
            obj->conn_promise.set_value(status == REDIS_OK);
        }
        // ----------------------------------------------------------------------------------
        void subscriber::disconnect_callback(UNUSED_ARG const redisAsyncContext * c , UNUSED_ARG int status)
        {
            auto* obj = (subscriber*)c->data;
            obj->connected = false;
        }
        // ----------------------------------------------------------------------------------
        void subscriber::auth_callback(UNUSED_ARG redisAsyncContext *c, void *r, UNUSED_ARG void *privdata)
        {
            auto *reply = (redisReply*)r;
            auto* obj = (subscriber*)c->data;

            bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
            obj->auth_promise.set_value(!failed);
        }
        // ----------------------------------------------------------------------------------
        bool subscriber::is_connected() const
        {
            return this->connected;
        }
        // ----------------------------------------------------------------------------------
        void subscriber::stop()
        {
            disconnect();
            runner.stop();
            runner.wait();
        }
        // --------------------------------------------------------------------------------------
        void subscriber::wait()
        {
            runner.wait();
        }
        // ======================================================================================
        static void tr_callback(UNUSED_ARG redisAsyncContext* c, void* r, UNUSED_ARG void* privdata)
        {
            auto* reply = (redisReply*) r;

            bool failed = (!reply || (reply->type == REDIS_REPLY_ERROR));
            if (failed)
            {
             //   LOG_ERROR("Failed to send data");
             // TODO
            }
        }
        // --------------------------------------------------------------------------------------
        publisher::publisher(const std::string& channel, const connection_parameters& cp)
        : m_name(channel),
          conn(cp),
          m_is_connected(false),
          redis_context(nullptr)
        {
            redis_context = redisConnect(conn.host.c_str(), conn.port);
            if (!redis_context || redis_context->err)
            {
                std::ostringstream os;
                os << "Failed to connect to REDIS : ";
                if (redis_context->errstr[0] != 0)
                {
                    os << redis_context->errstr;
                } else
                {
                    os << redis_context->err;
                }
                throw std::runtime_error(os.str());
            }
            m_runner = active_object::create().release();
        }
        // --------------------------------------------------------------------------------------
        bool publisher::auth()
        {
            redisReply *reply = nullptr;
            if (!conn.password.empty())
            {
                if (conn.username.empty())
                {
                    reply = (redisReply*)redisCommand(redis_context,  "AUTH %s",
                                 conn.password.c_str());
                }
                else
                {
                    reply = (redisReply*)redisCommand(redis_context, "AUTH %s %s",
                                 conn.username.c_str(), conn.password.c_str());
                }
            }
            if (reply)
            {
                bool failed = reply->type == REDIS_REPLY_ERROR;
                m_is_connected = !failed;
                freeReplyObject(reply);
            }
            else
            {
                m_is_connected = true;
            }
            return m_is_connected;
        }
        // --------------------------------------------------------------------------------------
        publisher::~publisher()
        {
            if (redis_context)
            {
                redisFree(redis_context);
            }
            delete m_runner;
        }
        // --------------------------------------------------------------------------------------
        bool publisher::is_connected() const
        {
            return m_is_connected;
        }
        // --------------------------------------------------------------------------------------
        void publisher::disconnect()
        {
        }
        // --------------------------------------------------------------------------------------
        void publisher::stop()
        {
            delete m_runner;
            m_runner = nullptr;
        }
        // --------------------------------------------------------------------------------------
        void publisher::wait()
        {

        }
        // --------------------------------------------------------------------------------------
        void publisher::send(const std::string& data)
        {
            send(tr_callback, nullptr, data);
        }
        // --------------------------------------------------------------------------------------
        void publisher::send(redisCallbackFn *fn, void *privdata, const std::string& data)
        {
            if (!is_connected())
            {
                return;
            }
            auto callable = [this, data, fn, privdata]() {
                redisReply* reply = (redisReply*)redisCommand(this->redis_context,
                             "PUBLISH %s %s", m_name.c_str(),
                                  data.c_str());
                fn(nullptr, reply, privdata);
                freeReplyObject(reply);
            };
            m_runner->post(callable);
        }
        // --------------------------------------------------------------------------------------
        void publisher::rpush(const std::string& array, const std::string& data)
        {
            if (!is_connected())
            {
                return;
            }
            auto callable = [this, array, data]() {
                auto* reply = (redisReply*)redisCommand(redis_context,
                                  "RPUSH %s %s", array.c_str(), data.c_str());
                freeReplyObject(reply);
            };
            m_runner->post(callable);
        }
        // --------------------------------------------------------------------------------------
        void publisher::rpush(const std::string& array, const std::string& data, const std::vector<char>& bindata)
        {
            if (!is_connected())
            {
                return;
            }
            auto callable = [this, array, data, bindata]() {
                auto* reply = (redisReply*)redisCommand(redis_context,
                                  "RPUSH %s %s %b", array.c_str(), data.c_str(), bindata.data(), bindata.size());
                freeReplyObject(reply);
            };
            m_runner->post(callable);
        }
        // --------------------------------------------------------------------------------------
        void publisher::llen(const std::string& array, redisCallbackFn *fn, void *privdata)
        {
            if (!is_connected())
            {
                return;
            }
            auto callable = [this, array, fn, privdata]() {
                auto* reply = (redisReply*)redisCommand(redis_context,
                                  "LLEN %s", array.c_str());
                fn(nullptr, reply, privdata);
                freeReplyObject(reply);
            };
            m_runner->post(callable);
        }
    } // ns redis
} // ns binadox
