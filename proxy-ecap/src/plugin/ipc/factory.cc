//
// Created by igor on 04/08/2020.
//

#include "plugin/ipc/factory.hh"
#include "plugin/ipc/redis_ipc.hh"
namespace binadox
{
    ipc* create_ipc (ipc* fallback, const redis::connection_parameters& cp)
    {
        return new redis::ipc(fallback, cp);
    }
} // ns binadox
