//
// Created by igor on 16/02/2021.
//

#include <pthread.h>
#include <event2/thread.h>
#include <event2/event.h>

#include <stdexcept>
#include "ev_util.hh"

static bool initialized = false;

namespace binadox
{
    void ev_init()
    {
        if (!initialized)
        {
            evthread_enable_lock_debugging();
            initialized = true;
            if (-1 == evthread_use_pthreads())
            {
                throw std::runtime_error("failed to initialize libevent pthreads");
            }
        }
    }
    // =======================================================================================
    struct ev_loop_thread::impl
    {
        impl()
        : done(false)
        {
            ev_init();
            base = event_base_new();
            if (!base)
            {
                throw std::runtime_error("failed to create event_base");
            }
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        }

        ~impl()
        {
            pthread_attr_destroy(&attr);
            event_base_free(base);
        }

        static void* runner(void *t)
        {
            event_base* event_loop = (event_base*)t;
            event_base_loop(event_loop, EVLOOP_NO_EXIT_ON_EMPTY);
            pthread_exit(nullptr);
        }

        void start()
        {
            int rc = pthread_create(&thread, &attr, runner, (void *)base);
            if (rc)
            {
                throw std::runtime_error("failed to create pthread evloop runner");
            }
        }

        void wait()
        {
            void* status;
            pthread_join(thread, &status);
            done = true;
        }

        event_base*    base;
        pthread_t      thread;
        pthread_attr_t attr;
        bool           done;

    };
    // =======================================================================================
    ev_loop_thread::ev_loop_thread()
    {
        pimpl = std::unique_ptr<impl>(new impl);
    }
    // ---------------------------------------------------------------------------------------
    ev_loop_thread::~ev_loop_thread()
    {
        stop();
        wait();
    }
    // ---------------------------------------------------------------------------------------
    event_base* ev_loop_thread::start()
    {
        event_base* ret = pimpl->base;
        pimpl->start();
        return ret;
    }
    // ---------------------------------------------------------------------------------------
    void ev_loop_thread::wait ()
    {
        if (!pimpl->done)
        {
            pimpl->wait();
        }
    }
    // ---------------------------------------------------------------------------------------
    void ev_loop_thread::stop()
    {
        if (!pimpl->done)
        {
            event_base_loopbreak(pimpl->base);
        }
    }
} // ns binadox