//
// Created by igor on 16/02/2021.
//

#ifndef BINADOX_ECAP_EV_UTIL_HH
#define BINADOX_ECAP_EV_UTIL_HH

#include <memory>

struct event_base;

namespace binadox
{
    void ev_init();

    class ev_loop_thread
    {
        struct impl;
    public:
        ev_loop_thread();
        ~ev_loop_thread();

        event_base* start();
        void wait ();
        void stop();
    private:
        std::unique_ptr<impl> pimpl;
    };
}

#endif //BINADOX_ECAP_EV_UTIL_HH
