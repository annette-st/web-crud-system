//
// Created by igor on 09/07/2020.
//

#ifndef BINADOX_ECAP_LOGGER_HH
#define BINADOX_ECAP_LOGGER_HH

#include <iostream>
#include <sstream>
#include <libecap/common/log.h>
#include <nonstd/string_view.hpp>

#include "backend/abstract_printer.hh"

namespace binadox
{
    class logger
    {
    public:
        static void print_caption();
        static nonstd::string_view get_source(nonstd::string_view file);

        static std::ostream* get_stream(libecap::LogVerbosity lvl);
        static void close_stream(std::ostream* os);

        static void prolog(libecap::LogVerbosity level, std::ostream* stream);
        static void epilog(std::ostream* stream);
    };

    void raise(const char* str, const char* file, int line);

} // ns binadox

#define dLOG_TRACE_FLAG (libecap::LogVerbosity)(libecap::ilNormal   | libecap::flApplication)
#define dLOG_ERROR_FLAG (libecap::LogVerbosity)(libecap::ilCritical | libecap::flApplication)
#define dLOG_DEBUG_FLAG (libecap::LogVerbosity)(libecap::ilDebug    | libecap::flApplication)

#define dLOCUS ::binadox::locus(::binadox::logger::get_source(__FILE__), __LINE__)

#define dLOG_TRACE(FLAG, ...)                                           \
    do {                                                                \
    std::ostream* stream = ::binadox::logger::get_stream(FLAG);         \
    if (stream)                                                         \
    {                                                                   \
        ::binadox::logger::prolog(FLAG, stream);                        \
        (*stream) << dLOCUS << " ";                                     \
        ::binadox::abstract_printer::print((*stream), ##__VA_ARGS__);   \
        ::binadox::logger::epilog(stream);                              \
        ::binadox::logger::close_stream(stream);                        \
    }                                                                   \
    break;                                                              \
    } while (true)

#define LOG_TRACE(...) dLOG_TRACE(dLOG_TRACE_FLAG, ##__VA_ARGS__)
#define LOG_ERROR(...) dLOG_TRACE(dLOG_ERROR_FLAG, ##__VA_ARGS__)
#define LOG_DEBUG(...) dLOG_TRACE(dLOG_DEBUG_FLAG, ##__VA_ARGS__)

#define ENFORCE(cond) ((cond) ? \
		(void)0 : \
		(void)::binadox::raise(#cond, __FILE__, __LINE__))


#define RAISE_EX(...)                                           \
    do {                                                        \
        std::ostringstream os;                                  \
        ::binadox::abstract_printer::print(os, ##__VA_ARGS__);  \
        ::binadox::raise(os.str().c_str(), __FILE__, __LINE__); \
        break;                                                  \
    } while(true)


#endif //BINADOX_ECAP_LOGGER_HH
