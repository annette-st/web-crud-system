//
// Created by igor on 04/08/2020.
//

#ifndef BINADOX_ECAP_FACTORY_HH
#define BINADOX_ECAP_FACTORY_HH

#include "backend/redis/redis_connection_parameters.hh"
#include <memory>

namespace binadox
{
    class ipc;
    ipc* create_ipc (ipc* fallback, const redis::connection_parameters& cp);
} // ns binadox

#endif //BINADOX_ECAP_FACTORY_HH
