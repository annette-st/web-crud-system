//
// Created by igor on 09/07/2020.
//
#include <ostream>
#include <string.h>
#include <libecap/common/named_values.h>
#include <libecap/common/options.h>
#include <libecap/common/name.h>
#include <libecap/common/area.h>
#include <libecap/common/autoconf.h>
#include <libecap/common/registry.h>

#include "plugin/service.hh"
#include "plugin/pipeline/script_ipc_pipeline.hh"
#include "plugin/logger.hh"
#include "plugin/conf/configurator.hh"
#include "plugin/conf/config_reader.hh"
#include "plugin/processors/response_processor.hh"
#include "plugin/processors/request_processor.hh"
#include "plugin/ipc/ipc.hh"
#include "plugin/fast_rand.hh"

namespace binadox
{

    static bool ipc_started = false;

    struct impl
    {
        impl(bool ais_request)
        : is_request(ais_request),
          was_shutdown(false)
        {
        }
        bool is_request;
        bool was_shutdown;
    };
    // ----------------------------------------------------------------------------------------------------
    service::service(bool is_request)
    {
        fast_rand_init();
        m_pimpl = new impl (is_request);

    }
    // ----------------------------------------------------------------------------------------------------
    service::~service()
    {
        shutdown();
        delete m_pimpl;
    }
    // ----------------------------------------------------------------------------------------------------
    // About
    std::string service::uri() const // unique across all vendors
    {
        if (m_pimpl->is_request)
        {
            return "ecap://www.binadox.com/ecap_binadox_send";
        } else {
            return "ecap://www.binadox.com/ecap_binadox_recv";
        }
    }
    // ----------------------------------------------------------------------------------------------------
    // changes with version and config
    std::string service::tag() const
    {
        return "0.0.1";
    }
    // ----------------------------------------------------------------------------------------------------
    // free-format info
    void service::describe(std::ostream& os) const
    {
        os << "Binadox Inc.";
    }
    // ----------------------------------------------------------------------------------------------------
    class config_visitor : public libecap::NamedValueVisitor
    {
    public:
        config_visitor(service& aSvc)
                : svc(aSvc)
        {}
        virtual void visit(const libecap::Name& name, const libecap::Area& value)
        {
            svc.setOne(name, value);
        }
        service& svc;
    };
    // ----------------------------------------------------------------------------------------------------
    // Configuration
    void service::configure(const libecap::Options& cfg)
    {
        config_visitor cfgtor(*this);
        cfg.visitEachOption(cfgtor);
    }
    // ----------------------------------------------------------------------------------------------------
    void service::reconfigure(const libecap::Options& cfg)
    {
        config_visitor cfgtor(*this);
        cfg.visitEachOption(cfgtor);
    }
    // ----------------------------------------------------------------------------------------------------
    void service::setOne(const libecap::Name& name, const libecap::Area& valArea)
    {
        const std::string value = valArea.toString();
        if (name == "config")
        {
            if (!get_config())
            {
                logger::print_caption();
            }
            config_init(read_config_file(valArea.toString()));

        }
    }
    // ----------------------------------------------------------------------------------------------------
    // Lifecycle
    // expect makeXaction() calls
    void service::start()
    {
        if (!ipc_started)
        {
            get_config()->get_ipc()->start();
            ipc_started = true;
        }
        libecap::adapter::Service::start();
    }
    // ----------------------------------------------------------------------------------------------------
    void service::stop() // no more makeXaction() calls until start()
    {
        shutdown();
        libecap::adapter::Service::stop();
    }
    // ----------------------------------------------------------------------------------------------------
    void service::retire() // no more makeXaction() calls
    {
        shutdown();
        libecap::adapter::Service::stop();
    }
    // ----------------------------------------------------------------------------------------------------
    void service::shutdown()
    {
        if (!ipc_started)
        {
            return;
        }
        if (!m_pimpl->was_shutdown)
        {
            m_pimpl->was_shutdown = true;
        } else
        {
            return;
        }
        ipc* obj = get_config()->get_ipc();
        if (obj)
        {
            obj->stop();
        }
        config_done();
    }
    // ----------------------------------------------------------------------------------------------------
    // Scope (XXX: this may be changed to look at the whole header)
    bool service::wantsUrl(const char* url) const
    {
        if (m_pimpl->is_request) {
            if (m_pimpl && get_config())
            {
                return request_processor::wants_url(url);
            }
        } else {
            return !script_ipc_pipeline::wants_url(url, strlen(url));
        }
        return true;
    }
    // ----------------------------------------------------------------------------------------------------
    libecap::adapter::Service::MadeXactionPointer service::makeXaction(libecap::host::Xaction* hostx)
    {
        if (m_pimpl->is_request) {
            return libecap::adapter::Service::MadeXactionPointer(
                    new request_processor(hostx));
        } else
        {
            return libecap::adapter::Service::MadeXactionPointer(
                    new response_processor(hostx));
        }
    }
}
// create the adapter and register with libecap to reach the host application
static const bool RegisteredReq = libecap::RegisterVersionedService(new binadox::service(true));
static const bool RegisteredResp = libecap::RegisterVersionedService(new binadox::service(false));


// =========================================================================================================
// UNITTEST REGISTRATION
// =========================================================================================================

#if defined(BINADOX_WITH_UNITTESTS)
#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
DOCTEST_SYMBOL_EXPORT void from_dll();   // to silence "-Wmissing-declarations" with GCC
DOCTEST_SYMBOL_EXPORT void from_dll() {} // force the creation of a .lib file with MSVC
#endif
