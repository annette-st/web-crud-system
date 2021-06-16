//
// Created by igor on 09/07/2020.
//

#include <libecap/common/autoconf.h>
#include <libecap/common/registry.h>
#include <libecap/host/host.h>
#include <libecap/common/errors.h>

#include <termcolor/termcolor.hpp>
#include "logger.hh"
#include "version_info.hh"

static std::size_t get_root_idx()
{
    nonstd::string_view sv(__FILE__);
    nonstd::string_view::size_type  pos = sv.find("logger.cc");
    return pos;
}

#define USE_SQUID_LOGGER

namespace binadox
{

    std::ostream* logger::get_stream(libecap::LogVerbosity lvl)
    {
#if defined(USE_SQUID_LOGGER)
        try
        {
            return libecap::MyHost().openDebug(lvl);
        } catch (...)
        {
            return nullptr;
        }
#else
        (void)lvl;
        return &std::cout;
#endif
    }

    void logger::close_stream(std::ostream* os)
    {
        if (os && (os->rdbuf() != std::cout.rdbuf()))
        {
            libecap::MyHost().closeDebug(os);
        }
    }

    void logger::print_caption()
    {
        std::ostream* os = logger::get_stream(dLOG_TRACE_FLAG);
        if (os)
        {
            *os << std::endl << termcolor::yellow << "HASH " << GitMetadata::CommitSHA1() << std::endl
               << "DATE " << GitMetadata::CommitDate() << std::endl
               << "SUBJECT " << GitMetadata::CommitSubject() << std::endl
               << termcolor::reset << std::endl;
            logger::close_stream(os);
        }
    }
    // -----------------------------------------------------------------------------------------
    void logger::prolog(libecap::LogVerbosity level, std::ostream* stream)
    {
        if (stream)
        {
            if (level.critical())
            {
                (*stream) << termcolor::red;
            } else
            {
                if (level.debugging())
                {
                    (*stream) << termcolor::magenta;
                } else
                {
                    (*stream) << termcolor::green;
                }
            }
            (*stream) << "[BINADOX] ";
        }
    }
    // -----------------------------------------------------------------------------------------
    void logger::epilog(std::ostream* stream)
    {
        if (stream)
        {
            (*stream) << termcolor::reset;
#if !defined(USE_SQUID_LOGGER)
            (*stream) << std::endl;
#endif
        }
    }
    // -----------------------------------------------------------------------------------------
    nonstd::string_view logger::get_source(nonstd::string_view file)
    {
        static std::size_t root_idx = get_root_idx();
        file.remove_prefix(root_idx);
        return file;
    }
    // -----------------------------------------------------------------------------------------
    void raise(const char* str, const char* file, int line)
    {
        auto flag = (libecap::LogVerbosity)(libecap::ilCritical | libecap::flApplication);
        std::ostream* stream = logger::get_stream(flag);
        if (stream)
        {
            logger::prolog(flag, stream);
            (*stream) << "Error at " << binadox::locus(logger::get_source(file), line) << " ";
            ::binadox::abstract_printer::print((*stream), str);
            ::binadox::logger::epilog(stream);
            ::binadox::logger::close_stream(stream);
        }
        libecap::Throw(str, file, line);
    }
} // ns binadox
