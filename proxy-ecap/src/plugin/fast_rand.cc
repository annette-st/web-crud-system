//
// Created by igor on 17/08/2020.
//

#include "plugin/fast_rand.hh"
#include <time.h>

namespace binadox
{
    void fast_rand_init(__uint128_t val)
    {
        if (!val)
        {
            g_lehmer64_state3 = time(NULL);
        } else
        {
            g_lehmer64_state3 = val;
        }
    }

    __uint128_t g_lehmer64_state3;
}
