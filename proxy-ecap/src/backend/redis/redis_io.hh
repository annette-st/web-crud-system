//
// Created by igor on 29/09/2020.
//

#ifndef BINADOX_ECAP_REDIS_IO_HH
#define BINADOX_ECAP_REDIS_IO_HH

#include <future>
#include <thread>
#include <mutex>
#include <vector>

#include "backend/ev_util.hh"
#include "backend/redis/redis_connection_parameters.hh"
#include "backend/active_object.hh"

struct redisAsyncContext;
struct redisContext ;
struct event_base;

namespace binadox
{
    namespace redis
    {
        class io
        {
        public:
            virtual ~io();

            virtual bool auth() = 0;
            virtual bool is_connected() const = 0;
            virtual void disconnect() = 0;
            virtual void stop() = 0;
            virtual void wait() = 0;
        };
        // =============================================================================================
        class publisher : public io
        {
        public:
            publisher(const std::string& channel, const connection_parameters& cp);
            ~publisher() override;
            void send(const std::string& data);
            void send(redisCallbackFn *fn, void *privdata, const std::string& data);
            void rpush(const std::string& array, const std::string& data);
            void rpush(const std::string& array, const std::string& data, const std::vector<char>& bindata);
            void llen(const std::string& array, redisCallbackFn *fn, void *privdata);

            bool auth() override;
            bool is_connected() const override;
            void disconnect() override;
            void stop() override;
            void wait() override;
        private:
            std::string m_name;
            active_object* m_runner;
            const connection_parameters& conn;
            bool m_is_connected;
            redisContext* redis_context;

        };
        // =============================================================================================
        class subscriber : public io
        {
        public:
            subscriber(const std::string& channel, const connection_parameters& cp);
            ~subscriber() override;
            void attach(redisCallbackFn *fn, void *privdata);

            bool auth() override;
            bool is_connected() const override;
            void disconnect() override;
            void stop() override;
            void wait() override;
        private:
            static void connect_callback(const redisAsyncContext *c, int status);
            static void disconnect_callback(const redisAsyncContext *c, int status);
            static void auth_callback(redisAsyncContext *c, void *r, void *privdata);


        private:
            std::string m_name;
            const connection_parameters& conn;
            event_base*        event_loop;
            ev_loop_thread     runner;
            redisAsyncContext* redis_context;

            std::promise<bool> conn_promise;
            std::promise<bool> auth_promise;

            bool connected;
        };
    }
} // ns binadox

#endif //BINADOX_ECAP_REDIS_IO_HH
