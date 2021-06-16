//
// Created by igor on 17/08/2020.
//

#ifndef BINADOX_ECAP_FAST_RAND_HH
#define BINADOX_ECAP_FAST_RAND_HH

#include <stdint.h>

// https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/

namespace binadox
{
    void fast_rand_init(__uint128_t val = 0);

    extern __uint128_t g_lehmer64_state3;

    inline unsigned long long fast_rand() {
        g_lehmer64_state3 *= 0xda942042e4dd58b5ull;
        return g_lehmer64_state3 >> 64;
    }

    inline void gen_random_string(char *s, const int len) {
        static const char alphanum[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

        for (int i = 0; i < len; ++i) {
            s[i] = alphanum[fast_rand() % (sizeof(alphanum) - 1)];
        }
    }
}


#endif //BINADOX_ECAP_FAST_RAND_HH
