//
// Created by igor on 13/08/2020.
//

#ifndef BINADOX_ECAP_SCRIPT_IPC_PIPELINE_HH
#define BINADOX_ECAP_SCRIPT_IPC_PIPELINE_HH

#include "plugin/pipeline/request_pipeline.hh"
#include "plugin/ipc/ipc.hh"

namespace binadox
{
    class script_ipc_pipeline : public request_pipeline
    {
    public:
        script_ipc_pipeline ();
        ~script_ipc_pipeline();

        static bool accept(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        static bool wants_url(const char* url, std::size_t url_len);
    private:
        virtual bool init(libecap::host::Xaction *hostx, const libecap::RequestLine* line);
        virtual libecap::shared_ptr<libecap::Message> generate_response();


        virtual void consume(const char* data, std::size_t size);
        virtual void finish ();

        virtual libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        virtual void update_transmitted_offset (std::size_t offs) ;
    private:
        ipc::message_type_t message_type;
        std::shared_ptr<std::string> input;
        std::string sending_buffer;
        std::size_t sending_offset;
        libecap::shared_ptr<libecap::Message> adapted;
    };
} // ns binadox

#endif //BINADOX_ECAP_SCRIPT_IPC_PIPELINE_HH
