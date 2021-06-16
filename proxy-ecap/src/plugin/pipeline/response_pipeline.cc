//
// Created by igor on 25/07/2020.
//

// curl 'https://www.kernel.org/3f785034-d25a-11ea-87d0-0242ac130003' -H 'User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:80.0) Gecko/20100101 Firefox/80.0' -H 'Accept: */*' -H 'Accept-Language: en-US,en;q=0.5' --compressed -H 'Referer: https://www.kernel.org/' -H 'Content-Type: application/x-www-form-urlencoded' -H 'Origin: https://www.kernel.org' -H 'Connection: keep-alive' -H 'Pragma: no-cache' -H 'Cache-Control: no-cache' --data-raw '{"type":"tracking_data","clientip":"127.0.0.1","login":null,"name":"ping","payload":{"tabid":"2ad53640-1dfb-2316-41e7-1bcc2c3e5c58","GUID":"123456","orig":"aHR0cHM6Ly93d3cua2VybmVsLm9yZw=="},"url":"aHR0cHM6Ly93d3cua2VybmVsLm9yZy8="}'
//#define PREPARE_DATA

#include <stdio.h>
#include <string.h>
#include <sstream>

#include <libecap/common/errors.h>
#include <libecap/common/header.h>

#include "plugin/pipeline/response_pipeline.hh"
#include "plugin/pipeline/zlc.hh"
#include "plugin/pipeline/script/script_writer.hh"
#include "plugin/pipeline/script/script.hh"
#include "plugin/conf/key_value_storage.hh"
#include "plugin/conf/includes_loader/abstract_includes_loader.hh"
#include "plugin/filter/tld.hh"

#include "plugin/constants.hh"
#include "backend/unused.hh"
#include "plugin/logger.hh"

#include "plugin/sniffer_db/sniffer_db.hh"
#include "plugin/sniffer_db/sniffer_meta_data.hh"

#include <nonstd/string_view.hpp>

namespace
{
    template <typename STRING>
    void inject_data(std::vector<char>& out, std::size_t pos, const STRING& what)
    {
        const std::size_t original_size = out.size();
        const std::size_t what_size = what.size();
        const std::size_t suffix_size = original_size - pos;

        out.resize(original_size + what_size);
        memmove(out.data() + pos + what_size, out.data() + pos, suffix_size);
        memcpy(out.data() + pos, what.data(), what_size);
    }
}

#if defined(PREPARE_DATA)
#define WRITE_ENABLED true
#else
#define WRITE_ENABLED false
#endif
namespace binadox
{
    std::string response_pipeline::default_nonce = "2726c7f26c";
    response_pipeline::response_pipeline(const pipeline_context& ctx)
            : context(ctx),
              is_write_enabled(WRITE_ENABLED),
              input_chunk_num(0),
              consume_state(eWAIT_FOR_BODY),
              input_offset(0),
              sending_offset(0),
              has_all_input(false),
              total_input(0),
              csp_point_begin(nullptr)
    {
        switch (context.encoding)
        {
            case eDEFLATE:
                decompressor = zlc::decompressor::raw();
                compressor = zlc::compressor::raw();
                break;
            case eGZIP:
                decompressor = zlc::decompressor::gzip();
                compressor = zlc::compressor::gzip(0);
                break;
            case eBR:
                decompressor = zlc::decompressor::brotli();
                compressor = zlc::compressor::brotli();
                break;
            default:
                decompressor = nullptr;
                compressor = nullptr;
                break;
        }

        if (is_write_enabled)
        {
            write_enc(context.encoding);
        }

        script_ctx = parse_csp(context.content_security_policy, context.content_security_policy_report, default_nonce);
    }
    // ---------------------------------------------------------------------------------------------
    response_pipeline::~response_pipeline()
    {
        delete compressor;
        delete decompressor;
    }
    // ---------------------------------------------------------------------------
    void response_pipeline::write_input(const char* data, std::size_t size)
    {
        std::ostringstream os;
        os << "/tmp/pipeline/" << input_chunk_num;
        FILE* f = fopen(os.str().c_str(), "wb");
        fwrite(data, size, 1, f);
        fclose(f);

    }
    // ---------------------------------------------------------------------------
    void response_pipeline::write_enc(content_encoding_t enc)
    {
        FILE* f = fopen("/tmp/pipeline/enc", "wb");
        std::string name = to_string(enc);
        fwrite(name.c_str(), name.size(), 1, f);
        fclose(f);
    }
    // ---------------------------------------------------------------------------
    std::size_t response_pipeline::get_last_input_length() const
    {
        return last_chunk_size;
    }
    // ---------------------------------------------------------------------------
    void response_pipeline::consume(const char* data, std::size_t size)
    {
        total_input += size;
        last_chunk_size = size;
        const bool transmit_prefix = (consume_state == eINJECTION_DONE);
        if (transmit_prefix)
        {
            input_buffer.resize(0);
            input_offset = 0;
        }

        std::size_t chunk_size = size;
        if (is_write_enabled)
        {
            write_input(data, size);
        }
        // form input
        std::vector<char> sniff_buff;

        if (decompressor)
        {
            std::size_t old_size = input_buffer.size();
            chunk_size = decompressor->decompress(data, size, input_buffer);
            if (context.sniffer_md)
            {
                sniff_buff.resize(chunk_size);
                memcpy(sniff_buff.data(), input_buffer.data() + old_size, chunk_size);
            }
        } else
        {
            if (context.sniffer_md)
            {
                sniff_buff.resize(size);
                memcpy(sniff_buff.data(), data, size);
            }
            if (!transmit_prefix)
            {
                // If no injection point found, accumulate data in the input buffer
                std::size_t sz = input_buffer.size();
                input_buffer.resize(sz + size);
                memcpy(input_buffer.data() + sz, data, size);
            } else
            {
                // If injection point found, bypass the data directly to the input buffer
                std::size_t sz = sending_buffer.size();
                sending_buffer.resize(sz + size);
                memcpy(sending_buffer.data() + sz, data, size);
            }
        }

        if (consume_state == eWAIT_FOR_BODY)
        {
            consume_prefix(chunk_size);
        }
        // form output
        std::size_t transmit_size = input_buffer.size() - input_offset;
        if (compressor)
        {
            compressor->compress(input_buffer.data() + input_offset, transmit_size, sending_buffer);
        } else
        {
            if (!transmit_prefix)
            {
                std::size_t sz = sending_buffer.size();
                sending_buffer.resize(sz + transmit_size);
                memcpy(sending_buffer.data() + sz, input_buffer.data() + input_offset, transmit_size);
            }
        }
        input_offset += transmit_size;

        if (context.sniffer_md)
        {
            get_config()->sniffer_database().put(*context.sniffer_md, input_chunk_num, sniff_buff);
        }

        input_chunk_num++;
    }
    // ---------------------------------------------------------------------------
    void response_pipeline::finish()
    {
        if (!has_all_input)
        {
            if (compressor)
            {
                compressor->finish(sending_buffer);
            }
        }
        if (context.sniffer_md)
        {
            LOG_TRACE("END transaction ", context.sniffer_md->get_transaction_id());
            get_config()->sniffer_database().transaction_finished(*context.sniffer_md);
        }
        has_all_input = true;
    }
    // ---------------------------------------------------------------------------
    libecap::Area response_pipeline::transmit(libecap::size_type offs, UNUSED_ARG libecap::size_type size)
    {
        const std::size_t offset = sending_offset + offs;
        const std::size_t to_transmit = sending_buffer.size() - offset;
        if (offset >= sending_buffer.size())
        {
            return libecap::Area();
        }
        return libecap::Area::FromTempBuffer(sending_buffer.data() + offset, to_transmit);
    }
    // ---------------------------------------------------------------------------
    bool response_pipeline::is_finished() const
    {
        if (sending_offset >= sending_buffer.size())
        {
            return has_all_input;
        }
        return false;
    }
    // ---------------------------------------------------------------------------
    void response_pipeline::update_transmitted_offset(std::size_t offs)
    {
        sending_offset += offs;
    }
    // ---------------------------------------------------------------------------
    void response_pipeline::consume_prefix(std::size_t size)
    {
        bool init_needed = (input_chunk_num == 0);

        if (init_needed)
        {
            html_scanner_init(&html_lexer, input_buffer.data(), input_buffer.size());
        } else
        {
            html_scanner_add_input(&html_lexer, input_buffer.data(), size);
        }

        while (true)
        {
            int rc = html_scan(&html_lexer);
            if (rc == HTML_SCANNER_END_OF_INPUT)
            {
                break;
            } else
            {
                if (rc == HTML_SCANNER_FOUND_TOKEN)
                {
                    char* injection_point = 0;
                    if (html_lexer.token_type == TOKEN_HTML_HEAD_END)
                    {
                        injection_point = const_cast<char*>(html_lexer.token_begin);
                        create_injected(injection_point, false);
                    } else
                    {
                        if (html_lexer.token_type == TOKEN_HTML_BODY_START)
                        {
                            injection_point = const_cast<char*>(html_lexer.token_begin);
                            create_injected(injection_point, true);
                        } else {
                            if (html_lexer.token_type == TOKEN_HTML_META)
                            {
                                csp_point_begin = const_cast<char*>(html_lexer.token_begin);
                            }
                        }

                    }
                    if (injection_point)
                    {
                        consume_state = eINJECTION_DONE;
                        return;
                    }
                }
            }
        }
    }
    // ----------------------------------------------------------------------------------
    std::size_t response_pipeline::update_inline_csp(std::size_t injection_offset)
    {
        if (!csp_point_begin)
        {
            return injection_offset;
        }
        std::size_t csp_offset = csp_point_begin - input_buffer.data();
        nonstd::string_view sv (input_buffer.data()+csp_offset, input_buffer.size() - csp_offset);
        std::size_t delta = 0;
        auto last_idx = sv.find(">");
        if (last_idx != nonstd::string_view::npos)
        {
            auto last_q_idx = sv.rfind("\"", last_idx);
            nonstd::string_view csp_data(sv.data(), last_q_idx);

            csp_header hdr = csp_header::parse(csp_data.data(), csp_data.data() + csp_data.size());
            script_ctx = parse_csp(&hdr, nullptr, default_nonce);
            update_csp_header (script_ctx, &hdr);

            std::string new_csp_header = hdr.to_string();

            auto new_csp_header_size = new_csp_header.size();
            const std::size_t old_data_size = csp_data.size();
            for (std::size_t i = new_csp_header_size; i<old_data_size; i++)
            {
                new_csp_header += " ";
            }
            new_csp_header_size = new_csp_header.size();

            memcpy(input_buffer.data() + csp_offset, new_csp_header.c_str(), old_data_size);

            delta = new_csp_header_size - old_data_size;
            nonstd::string_view suffix(new_csp_header.c_str() + old_data_size, delta);
            inject_data(input_buffer, csp_offset + old_data_size, suffix);

        }
        if (csp_offset < injection_offset)
        {
            injection_offset = injection_offset + delta;
        }
        return injection_offset;
    }
    // ----------------------------------------------------------------------------------
    void response_pipeline::create_injected(char* injection_point, bool with_head)
    {
        std::size_t injection_offset = injection_point - input_buffer.data();
        injection_offset = update_inline_csp(injection_offset);

        script_writer wr(with_head);
        wr.write_comment("BINADOX");
        if (script_ctx.valid && script_ctx.need_nonce)
        {
            wr.start_script(script_ctx.nonce);
        } else {
            wr.start_script();
        }
        if (csp_point_begin)
        {
            script_ctx.valid = false; // do not update header
        }
        if (get_config())
        {
            nonstd::string_view orig_domain;
            std::string domain = get_tld_from_url(context.destination_url, orig_domain);

            std::string payload = "{}; \n /* Can not find configuration for: "
                    + context.destination_url + "->" + domain + " */";
            LOG_TRACE("Loading audit config for domain ", context.destination_url, " -> ", orig_domain);
            get_config()->get_key_value_storage().get_with_meta(domain, audit_config, payload);

            the_script.set(audit_config_template, payload);
            the_script.set(login_scripts_template, get_config()->login_config().get_by_domain(orig_domain));
            the_script.set(onload_template, get_config()->onload_config().get_by_domain(orig_domain));

            wr.stream() << the_script << "\n";

            wr << "__name_space__ ("
               << "\"" << context.origin_ip << "\""
               << ", \"" << context.destination_url << "\""
               << ", \"" << context.destination_url << "/" << acceptor_uuid << "\""
               << ", \"" << get_config()->get_workspace() << "\""
               << ");";
        }

        wr.write_line("");
        wr.end_script();
        std::string injected_code = wr.finalize();

        inject_data(input_buffer, injection_offset, injected_code);
    }
    // ---------------------------------------------------------------------------------------------------------
    void response_pipeline::update_headers (libecap::shared_ptr<libecap::Message>& adapted)
    {
        // Remove ContentLength header from the response
        adapted->header().removeAny(libecap::headerContentLength);

        if (context.content_security_policy)
        {
            update_csp_header(script_ctx, context.content_security_policy);
            adapted->header().removeAny(headerCSP);
            const std::string new_csp = context.content_security_policy->to_string();
            if (!new_csp.empty())
            {
                adapted->header().add(headerCSP, libecap::Area::FromTempString(new_csp));
            }
        }
        else
        {
            if (context.content_security_policy_report)
            {
                update_csp_header(script_ctx, context.content_security_policy_report);
                adapted->header().removeAny(headerCSPReport);
                const std::string new_csp = context.content_security_policy_report->to_string();
                if (!new_csp.empty())
                {
                    adapted->header().add(headerCSPReport, libecap::Area::FromTempString(new_csp));
                }
            }
        }

        adapted->header().removeAny(accessControlAllowOrigin);
        adapted->header().add(accessControlAllowOrigin, libecap::Area::FromTempString("*"));
        if (!context.xframeoptions.empty())
        {
            adapted->header().removeAny(context.xframeoptions);
        }
        if (!context.xxssprotection.empty())
        {
            adapted->header().removeAny(context.xxssprotection);
        }
        /*
        if (!context.addon_origin.empty())
        {
            adapted->header().removeAny(accessControlAllowOrigin);
            if (context.access_allow_origin.empty())
            {
                adapted->header().add(accessControlAllowOrigin, libecap::Area::FromTempString(context.addon_origin));
            }
            else
            {
                adapted->header().add(accessControlAllowOrigin, libecap::Area::FromTempString("*"));
            }
        }
         */
    }
    // --------------------------------------------------------------------------------------------------------
} // ns binadox

// ===================================================================================================
#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

namespace
{
    class simple_reader
    {
    public:
        simple_reader (const std::string name)
        : root(name)
        {

        }
        std::string read(const std::string& name) const
        {
            std::ostringstream os;
            os << DATA_ROOT << "/" << root << "/" << name;
            FILE* f = fopen(os.str().c_str(), "rb");
            if (!f)
            {
                return "";
            }
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);

            std::string ret;
            ret.resize(sz);
            fread(const_cast<char*>(ret.data()), sz, 1, f);
            fclose(f);
            return ret;
        }

        std::vector<char> read(int c, zlc::decompressor* decompressor, bool& ok) const
        {
            std::vector<char> ret;
            std::ostringstream os;
            os << DATA_ROOT << "/" << root << "/" << c;
            FILE* f = fopen(os.str().c_str(), "rb");
            if (!f)
            {
                ok = false;
                return ret;
            }
            ok = true;
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);
            ret.resize(sz);
            fread(ret.data(), sz, 1, f);
            fclose(f);
            if (decompressor)
            {
                std::vector<char> out;
                decompressor->decompress(ret.data(), ret.size(), out);
                return out;
            }

            return ret;
        }
    private:
        std::string root;
    };

    class data_reader : public simple_reader
    {
    public:
        data_reader(const std::string& name)
        : simple_reader(name),
          decompressor(0),
          chunk(0)
        {
            std::string enc = read("enc");
            if (enc.empty())
            {
                throw std::runtime_error("no enc");
            }
            if (enc == binadox::to_string(binadox::eGZIP))
            {
                decompressor = zlc::decompressor::gzip();
            } else
            {
                if (enc == binadox::to_string(binadox::eDEFLATE))
                {
                    decompressor = zlc::decompressor::raw();
                } else
                {
                    if (enc == binadox::to_string(binadox::eBR))
                    {
                        decompressor = zlc::decompressor::brotli();
                    } else
                    {
                        decompressor = 0;
                    }
                }
            }
        }

        ~data_reader()
        {
            delete decompressor;
        }

        std::string read_all() const
        {
            std::string ret;
            int i = 0;
            while (true)
            {
                bool ok;
                std::vector<char> d = read(i++, decompressor, ok);
                if (!ok)
                {
                    break;
                }
                std::string s(d.data(), d.size());
                ret += s;
            }
            return ret;
        }

        std::vector<char> read_next(bool& ok)
        {
            return read(chunk++, decompressor, ok);
        }

    private:
        zlc::decompressor* decompressor;
        int chunk;
    };
}


TEST_CASE ("Test inject data")
{
    std::string orig("QWERTYUIOPASDFGHJKLZXCVBNM");
    std::vector<char> out(orig.size());
    memcpy(out.data(), orig.c_str(), orig.size());
    std::string what("12345678900987654321");

    inject_data(out, 3, what);
    REQUIRE(out.size() == what.size() + orig.size());

    std::string result = "";

    for (std::size_t i = 0; i < out.size(); i++)
    {
        result += out[i];
    }

    std::string e("QWE12345678900987654321RTYUIOPASDFGHJKLZXCVBNM");
    REQUIRE(e == result);
}

namespace
{
    bool find_csp(const std::string& name)
    {
        simple_reader dr("html");
        std::string data = dr.read(name);

        html_scanner html_lexer;
        html_scanner_init(&html_lexer, data.c_str(), data.size());


        while (true)
        {
            int rc = html_scan(&html_lexer);
            if (rc == HTML_SCANNER_END_OF_INPUT)
            {
                return false;
            } else
            {
                if (rc == HTML_SCANNER_FOUND_TOKEN)
                {

                    if (html_lexer.token_type == TOKEN_HTML_HEAD_END)
                    {
                     //   return true;
                    } else
                    {
                        if (html_lexer.token_type == TOKEN_HTML_BODY_START)
                        {
                            //     return true;
                        } else {
                            if (html_lexer.token_type == TOKEN_HTML_META)
                            {
                                return true;
                            }
                        }

                    }
                }
            }
        }
        return false;
    }
    // --------------------------------------------------------------------------------------------------
    bool injection_found_all(const std::string& name)
    {
        data_reader dr(name);
        std::string data = dr.read_all();

        html_scanner html_lexer;
        html_scanner_init(&html_lexer, data.c_str(), data.size());


        while (true)
        {
            int rc = html_scan(&html_lexer);
            if (rc == HTML_SCANNER_END_OF_INPUT)
            {
                return false;
            } else
            {
                if (rc == HTML_SCANNER_FOUND_TOKEN)
                {

                    if (html_lexer.token_type == TOKEN_HTML_HEAD_END)
                    {
                        return true;
                    } else
                    {
                        if (html_lexer.token_type == TOKEN_HTML_BODY_START)
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    // -------------------------------------------------------------------------------------------------
    bool injection_found_chunked(const std::string& name)
    {
        data_reader dr(name);

        html_scanner html_lexer;

        bool first_time = true;
        std::vector<char> data;
        while (true)
        {
            bool ok;
            std::vector<char> chunk = dr.read_next(ok);

            if (!ok)
            {
                return false;
            }

            data.insert(data.end(), chunk.begin(), chunk.end());

            if (first_time)
            {
                first_time = false;
                html_scanner_init(&html_lexer, data.data(), chunk.size());
            } else
            {
                html_scanner_add_input(&html_lexer, data.data(), chunk.size());
            }
            while (true)
            {
                int rc = html_scan(&html_lexer);
                if (rc == HTML_SCANNER_END_OF_INPUT)
                {
                    break;
                }
                if (rc == HTML_SCANNER_FOUND_TOKEN)
                {

                    if (html_lexer.token_type == TOKEN_HTML_HEAD_END)
                    {
                        return true;
                    } else
                    {
                        if (html_lexer.token_type == TOKEN_HTML_BODY_START)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
} // anon ns

TEST_SUITE ("HTML parser")
{
    TEST_CASE ("start.atlassian.com - meta")
    {
        REQUIRE(find_csp("start.atlassian.com.html"));
    }

    TEST_CASE ("login.salesforce.com - chunked")
    {
        REQUIRE(injection_found_chunked("login.salesforce.com"));
    }

    TEST_CASE ("login.salesforce.com")
    {
        REQUIRE(injection_found_all("login.salesforce.com"));
    }

    TEST_CASE ("mignews.com")
    {
        REQUIRE(injection_found_all("mignews.com"));
    }
    TEST_CASE ("cnn.com")
    {
        REQUIRE(injection_found_all("cnn.com"));
    }
    TEST_CASE ("ynet.co.il")
    {
        REQUIRE(injection_found_all("ynet.co.il"));
    }
    TEST_CASE ("mignews.com - chunked")
    {
        REQUIRE(injection_found_chunked("mignews.com"));
    }
    TEST_CASE ("cnn.com - chunked")
    {
        REQUIRE(injection_found_chunked("cnn.com"));
    }

    TEST_CASE ("ynet.co.il - chunked")
    {
        REQUIRE(injection_found_chunked("ynet.co.il"));
    }


}

namespace binadox
{
    class pipeline_tester : public simple_reader
    {
    public:
        pipeline_tester (const std::string& name)
        : simple_reader(name)
        {
            std::string enc = read("enc");
            if (enc.empty())
            {
                throw std::runtime_error("no enc");
            }
            if (enc == binadox::to_string(binadox::eGZIP)) {
                consumer = new response_pipeline(binadox::pipeline_context(binadox::eGZIP));
            } else if (enc == binadox::to_string(binadox::eDEFLATE))
                {
                    consumer = new response_pipeline(binadox::pipeline_context(binadox::eDEFLATE));
                } else {
                    consumer = new response_pipeline(binadox::pipeline_context(binadox::eNONE));
                }
        }

        ~pipeline_tester()
        {
            delete consumer;
        }

        bool test_injection()
        {
            int chunk = 0;
            while (true)
            {
                bool ok;
                std::vector<char> raw_data = read(chunk++, 0, ok);
                if (!ok)
                {
                    return false;
                }
                consumer->consume(raw_data.data(), raw_data.size());
                if (consumer->consume_state == response_pipeline::eINJECTION_DONE)
                {
                    return true;
                }
                libecap::Area a = consumer->transmit(0, 0xFFFFFFFF);
                consumer->update_transmitted_offset(a.size);
            }
            return false;
        }
    private:
        response_pipeline* consumer;
    };
}

TEST_SUITE("pipeline - injection")
{
    TEST_CASE("mignews.com")
    {
        binadox::pipeline_tester tester("mignews.com");
        bool rc = tester.test_injection();
        REQUIRE(rc);
    }
    TEST_CASE("cnn.com")
    {
        binadox::pipeline_tester tester("cnn.com");

        bool rc = tester.test_injection();
        REQUIRE(rc);
    }
    TEST_CASE("ynet.co.il")
    {
        binadox::pipeline_tester tester("ynet.co.il");
        bool rc = tester.test_injection();
        REQUIRE(rc);
    }
    TEST_CASE("login.salesforce.com")
    {
        binadox::pipeline_tester tester("login.salesforce.com");
        bool rc = tester.test_injection();
        REQUIRE(rc);
    }
}

#endif
