//
// Created by igor on 25/07/2020.
//

#ifndef BINADOX_ECAP_RESPONSE_PIPELINE_HH
#define BINADOX_ECAP_RESPONSE_PIPELINE_HH

#include <libecap/common/area.h>
#include <libecap/common/message.h>

#include <string>
#include <vector>

#include "plugin/pipeline/pipeline.hh"
#include "plugin/pipeline/pipeline_context.hh"
#include "plugin/pipeline/html/html_scanner.h"
#include "plugin/pipeline/csp/csp_parser.hh"
#include "plugin/pipeline/script/script.hh"

namespace zlc
{
    class decompressor;
    class compressor;
}

namespace binadox
{
#if defined(BINADOX_WITH_UNITTESTS)
    class pipeline_tester;
#endif

    class response_pipeline : public pipeline
    {
#if defined(BINADOX_WITH_UNITTESTS)
        friend class pipeline_tester;
#endif
    public:
        response_pipeline(const pipeline_context& ctx);
        virtual ~response_pipeline();

        void consume(const char* data, std::size_t size);
        void finish ();
        void update_headers (libecap::shared_ptr<libecap::Message>& adapted);

        libecap::Area transmit(libecap::size_type offset, libecap::size_type size);
        void update_transmitted_offset (std::size_t offs);
        std::size_t get_last_input_length() const;
        bool is_finished() const;
    protected:
        zlc::decompressor* decompressor;
        zlc::compressor*   compressor;
        pipeline_context   context;
    private:
        bool is_write_enabled;
        std::size_t input_chunk_num;
        void write_input(const char* data, std::size_t size);
        void write_enc(content_encoding_t enc);
    private:
        enum consume_state_t
        {
            eWAIT_FOR_BODY,
            eINJECTION_DONE
        };

        consume_state_t consume_state;

        html_scanner  html_lexer;
        std::vector<char> input_buffer;
        std::size_t input_offset;

        std::vector<char> sending_buffer;
        std::size_t sending_offset;
        bool has_all_input;
        std::size_t total_input;
        std::size_t last_chunk_size;

        static std::string default_nonce;
        script the_script;

        const char* csp_point_begin;
    private:
        void consume_prefix (std::size_t size);
        void create_injected(char* injection_point, bool with_head);
        std::size_t update_inline_csp(std::size_t injection_offset);
    private:
        script_gen_context script_ctx;
    };
} // ns binadox


#endif //BINADOX_ECAP_RESPONSE_PIPELINE_HH
