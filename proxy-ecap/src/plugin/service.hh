//
// Created by igor on 09/07/2020.
//

#ifndef BINADOX_ECAP_SERVICE_HH
#define BINADOX_ECAP_SERVICE_HH

#include <libecap/adapter/service.h>

namespace binadox
{

    class config_visitor;
    class event_loop;
    struct impl;

    class service : public libecap::adapter::Service
    {
        friend class config_visitor;

    public:
        service(bool is_request);
        ~service();
    private:
        // About
        virtual std::string uri() const; // unique across all vendors
        virtual std::string tag() const; // changes with version and config
        virtual void describe(std::ostream& os) const; // free-format info

        // Configuration
        virtual void configure(const libecap::Options& cfg);
        virtual void reconfigure(const libecap::Options& cfg);
        void setOne(const libecap::Name& name, const libecap::Area& valArea);
        const std::string mode; // REQMOD or RESPMOD (for unique service URI)

        // Lifecycle
        virtual void start(); // expect makeXaction() calls
        virtual void stop(); // no more makeXaction() calls until start()
        virtual void retire(); // no more makeXaction() calls

        // Scope (XXX: this may be changed to look at the whole header)
        virtual bool wantsUrl(const char* url) const;

        // Work
        virtual libecap::adapter::Service::MadeXactionPointer makeXaction(libecap::host::Xaction* hostx);
    private:
        void shutdown();
    private:
        impl* m_pimpl;
    };
} // ns binadox

#endif //BINADOX_ECAP_SERVICE_HH
