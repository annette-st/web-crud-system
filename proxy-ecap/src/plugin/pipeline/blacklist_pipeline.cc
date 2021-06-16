//
// Created by igor on 15/08/2020.
//
#include <libecap/host/xaction.h>
#include <libecap/host/host.h>
#include <libecap/common/autoconf.h>
#include <libecap/common/registry.h>
#include "plugin/pipeline/blacklist_pipeline.hh"
#include "plugin/constants.hh"
#include "backend/unused.hh"

namespace binadox
{
    blacklist_pipeline::blacklist_pipeline ()
    : send(false)
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    blacklist_pipeline::~blacklist_pipeline()
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    void blacklist_pipeline::consume(const char* /*data*/, std::size_t /*size*/)
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    void blacklist_pipeline::finish ()
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    bool blacklist_pipeline::init(UNUSED_ARG libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
            // use ECAP content satisfaction to forge the message
            adapted = libecap::MyHost().newResponse();
            libecap::StatusLine &statusLine = dynamic_cast<libecap::StatusLine&>(adapted->firstLine());
            statusLine.statusCode(403);
            statusLine.reasonPhrase(libecap::Area::FromTempString("Forbidden"));
            adapted->header().add(headerContentType, libecap::Area::FromTempString("text/html"));
            adapted->addBody();

            error_page = libecap::Area::FromTempString(get_config()->get_blacklist_page(line->uri()));
            return true;
    }
    // -----------------------------------------------------------------------------------------------------------
    libecap::Area blacklist_pipeline::transmit(UNUSED_ARG libecap::size_type offset, UNUSED_ARG libecap::size_type size)
    {
        if (!send)
        {
            return error_page;
        }
        return libecap::Area();
    }
    // -----------------------------------------------------------------------------------------------------------
    void blacklist_pipeline::update_transmitted_offset (UNUSED_ARG std::size_t offs)
    {
        send = true;
    }
    // -----------------------------------------------------------------------------------------------------------
    std::size_t blacklist_pipeline::get_last_input_length() const
    {
        return error_page.size;
    }
    // -----------------------------------------------------------------------------------------------------------
    bool blacklist_pipeline::accept(UNUSED_ARG libecap::host::Xaction *hostx, const libecap::RequestLine* line)
    {
        return get_config()->is_blacklisted(line->uri());
    }
    // -----------------------------------------------------------------------------------------------------------
    libecap::shared_ptr<libecap::Message> blacklist_pipeline::generate_response()
    {
        return adapted;
    }
    // -----------------------------------------------------------------------------------------------------------
    bool blacklist_pipeline::wants_url(UNUSED_ARG const char* url, UNUSED_ARG std::size_t url_len)
    {
        return get_config() && get_config()->has_blacklist();
    }
} // ns binadox
