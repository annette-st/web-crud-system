//
// Created by igor on 23/08/2020.
//

#ifndef BINADOX_ECAP_RAISE_HH
#define BINADOX_ECAP_RAISE_HH

#include "backend/abstract_printer.hh"
#include <sstream>
#include <stdexcept>

#define RAISE(...)                                                  \
do {                                                                \
    std::ostringstream os;                                          \
    ::binadox::locus loc(__FILE__, __LINE__);                       \
    os << "Error at " << loc << " ";                                \
    ::binadox::abstract_printer::print(os, ##__VA_ARGS__);          \
    throw std::runtime_error(os.str());                             \
} while(true)


#endif //BINADOX_ECAP_RAISE_HH
