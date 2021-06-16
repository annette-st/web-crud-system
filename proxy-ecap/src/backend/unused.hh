//
// Created by igor on 17/08/2020.
//

#ifndef BINADOX_ECAP_UNUSED_HH
#define BINADOX_ECAP_UNUSED_HH

#if (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#       define PGM_GNUC_UNUSED         __attribute__((__unused__))
#else
#       define PGM_GNUC_UNUSED
#endif

#define UNUSED_ARG PGM_GNUC_UNUSED

#endif //BINADOX_ECAP_UNUSED_HH
