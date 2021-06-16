//
// Created by igor on 04/08/2020.
//

#ifndef BINADOX_ECAP_REDIS_CONNECTION_PARAMETERS_HH
#define BINADOX_ECAP_REDIS_CONNECTION_PARAMETERS_HH

#include <string>

namespace binadox
{
    namespace redis
    {
        struct connection_parameters
        {
            std::string host;
            int port;
            std::string username;
            std::string password;
        };
    } // ns redis
} // ns binadox

#endif //BINADOX_ECAP_REDIS_CONNECTION_PARAMETERS_HH
