//
// Created by igor on 13/07/2020.
//

#ifndef BINADOX_ECAP_BASIC_PROCESSOR_HH
#define BINADOX_ECAP_BASIC_PROCESSOR_HH

#include <libecap/adapter/xaction.h>
#include <libecap/adapter/service.h>
#include <libecap/common/header.h>
#include <libecap/common/memory.h>
#include <libecap/common/area.h>

#include "plugin/conf/configurator.hh"
#include "plugin/processors/nanotimer.hh"
#include "plugin/pipeline/pipeline.hh"

#include <nonstd/string_view.hpp>

namespace binadox
{
    class basic_processor : public libecap::adapter::Xaction
    {
    public:
        basic_processor(libecap::host::Xaction *x);
        virtual ~basic_processor();
    protected:
        // meta-information for the host transaction
        virtual const libecap::Area option(const libecap::Name &name) const;
        virtual void visitEachOption(libecap::NamedValueVisitor &visitor) const;

        // lifecycle
        virtual void start();

        // This method is called from start. If false returned, no further handling will be performed
        // returns false by default
        virtual bool on_start_request(const libecap::RequestLine* line, const nonstd::string_view& domain);
        // This method is called from start. If false returned, no further handling will be performed
        // returns false by default
        virtual bool on_start_response(const libecap::StatusLine* line, const libecap::RequestLine* requestLine,
                                       const nonstd::string_view& domain);

        void set_consumer(pipeline* consumer);
        void set_adapted(libecap::shared_ptr<libecap::Message> msg);

        virtual void forge_headers(libecap::shared_ptr<libecap::Message> msg);

        virtual void on_stop ();
        virtual void stop();

        // adapted body transmission control
        virtual void abDiscard();
        virtual void abMake();
        virtual void abMakeMore();
        virtual void abStopMaking();

        // adapted body content extraction and consumption
        virtual libecap::Area abContent(libecap::size_type offset, libecap::size_type size);
        virtual void abContentShift(libecap::size_type size);

        // virgin body state notification
        virtual void noteVbContentDone(bool atEnd);
        virtual void noteVbContentAvailable();

    protected:
        // should be called from start method if no handling is needed
        void stopAction ();

        void stopVb(); // stops receiving vb (if we are receiving it)
        libecap::host::Xaction *lastHostCall(); // clears hostx
    protected:
        libecap::host::Xaction *hostx; // Host transaction rep
        typedef enum { opUndecided, opOn, opComplete, opNever } OperationState;

        OperationState receivingVb;
        OperationState sendingAb;

    private:
        plf::nanotimer timer;
        std::string url;

        enum process_status_t
        {
            eNONE,
            eREQUEST,
            eRESPONSE
        };
        process_status_t processed;
        pipeline* consumer;
        libecap::shared_ptr<libecap::Message> adapted;
    };
}

#endif //BINADOX_ECAP_BASIC_PROCESSOR_HH
