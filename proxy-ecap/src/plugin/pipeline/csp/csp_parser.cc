//
// Created by igor on 29/07/2020.
//

#include "plugin/pipeline/csp/csp_parser.hh"
#include "plugin/constants.hh"


namespace binadox
{
    script_gen_context::script_gen_context()
    : valid(false),
      need_nonce(false),
      has_unsafe_inline(false)
    {

    }
    // ------------------------------------------------------------------------------------
    void init_script_gen_context(const csp_header *header, script_gen_context& script_ctx, const std::string& default_nonce)
    {
        if (!header)
        {
            return;
        }
        script_ctx.valid = true;

        if (header->has_keyword(csp_header::SCRIPT_SRC, unsafe_inline))
        {
            script_ctx.has_unsafe_inline = true;
        } else {
            if (header->has_keyword(csp_header::DEFAULT_SRC, unsafe_inline))
            {
                script_ctx.has_unsafe_inline = true;
            }
        }
        const csp_header::nonce_info_t ninf =  header->get_nonce_policy();
        /*
        if (!script_ctx.has_unsafe_inline && (ninf.first != csp_header::eNONE))
        {
            script_ctx.need_nonce = true;
            script_ctx.nonce = default_nonce;
        }
         */
        if (ninf.first != csp_header::eNONE)
        {
            if (ninf.first == csp_header::eSELF && script_ctx.has_unsafe_inline)
            {
                script_ctx.need_nonce = false;
            }
            else
            {
                script_ctx.need_nonce = true;
                script_ctx.nonce = default_nonce;
            }
        }
        else
        {
            script_ctx.need_nonce = false;
        }
    }
    // ----------------------------------------------------------------------------------------
    script_gen_context parse_csp(const csp_header* csp, const csp_header* csp_report, const std::string& default_nonce)
    {
        script_gen_context ctx;
        if (csp)
        {
            init_script_gen_context(csp, ctx, default_nonce);
            ctx.has_frame_ancestors = csp->has_section(csp_header::FRAME_ANCSETORS);
        } else {
            init_script_gen_context(csp_report, ctx, default_nonce);
        }

        return ctx;
    }
    // ----------------------------------------------------------------------------------------
    void update_csp_header (const script_gen_context& ctx, csp_header* csp)
    {
        if (!ctx.valid)
        {
            return;
        }

        if (ctx.need_nonce)
        {
            csp->add_nonce(ctx.nonce);
        }

        if (ctx.has_frame_ancestors)
        {
            csp->remove_frame_ancestors();
        }

    }
} // ns binadox

#if defined(BINADOX_WITH_UNITTESTS)

#include <doctest.h>

binadox::script_gen_context gen_context(const std::string& raw)
{
    binadox::csp_header h = binadox::csp_header::parse(raw.c_str(), raw.c_str() + raw.size());
    return binadox::parse_csp(&h, nullptr, "AAA");
}

TEST_SUITE("CSP - handling")
{
    TEST_CASE("https://edition.cnn.com")
    {
        binadox::script_gen_context ctx = gen_context("default-src 'self' blob: https://*.cnn.com:* http://*.cnn.com:* *.cnn.io:* *.cnn.net:* *.turner.com:* *.turner.io:* *.ugdturner.com:* courageousstudio.com *.vgtf.net:*; script-src 'unsafe-eval' 'unsafe-inline' 'self' *; style-src 'unsafe-inline' 'self' blob: *; child-src 'self' blob: *; frame-src 'self' *; object-src 'self' *; img-src 'self' data: blob: *; media-src 'self' data: blob: *; font-src 'self' data: *; connect-src 'self' *; frame-ancestors 'self' https://*.cnn.com:* http://*.cnn.com https://*.cnn.io:* http://*.cnn.io:* *.turner.com:* courageousstudio.com;");
        REQUIRE(ctx.valid);
        REQUIRE_FALSE(ctx.need_nonce);
    }

    TEST_CASE("https://start.atlassian.com")
    {
        binadox::script_gen_context ctx = gen_context("object-src 'self';base-uri 'self';script-src 'unsafe-inline' 'self' https://uchi-ui.us-east-1.prod.public.atl-paas.net 'sha256-Mx03aoM/NZpZUPCAqCQhcag5i0JoZ4Vt8s53FJDyhpE=' 'sha256-ZPjZ08mmuvL0lkfwRqArCVKd4jZCkFU/uoFd7nSsyfk=' metal.prod.atl-paas.net ;worker-src 'self' blob:");
        REQUIRE(ctx.valid);
        REQUIRE(ctx.need_nonce);
    }

    TEST_CASE("https://id.atlassian.com")
    {
        binadox::script_gen_context ctx = gen_context("object-src 'none'; frame-ancestors 'self'; script-src 'unsafe-eval' 'nonce-uNe+Q+9DSoLjBQY8vRlMesjyHqs=' 'unsafe-inline' https: http: 'strict-dynamic'; base-uri 'self'; report-uri https://web-security-reports.services.atlassian.com/csp-report/aa");
        REQUIRE(ctx.valid);
        REQUIRE(ctx.need_nonce);
    }

    TEST_CASE("um5.lightning.force.com/lightning/page/home")
    {
        binadox::script_gen_context ctx = gen_context("default-src 'self'; script-src 'self' 'nonce-bdb5e024-0126-8e0d-da05-f6c4de4766d5' chrome-extension: 'unsafe-inline' 'unsafe-eval' *.canary.lwc.dev *.um5.visual.force.com https://ssl.gstatic.com/accessibility/ https://static.lightning.force.com https://*.salesforceliveagent.com/; object-src 'self' https://c.um5.content.force.com; style-src 'self' blob: chrome-extension: 'unsafe-inline' *.um5.visual.force.com https://c.um5.content.force.com; img-src 'self' http: https: data: blob: *.um5.visual.force.com; media-src 'self' *.um5.visual.force.com https://c.um5.content.force.com blob:; frame-ancestors 'self'; frame-src https: mailto: *.um5.visual.force.com; font-src 'self' https: data: *.um5.visual.force.com; connect-src 'self' https://api.bluetail.salesforce.com https://staging.bluetail.salesforce.com https://preprod.bluetail.salesforce.com blob: *.um5.visual.force.com https://c.um5.content.force.com https://um5.salesforce.com");
        REQUIRE(ctx.valid);
        REQUIRE(ctx.need_nonce);
    }

    TEST_CASE("kernel.org")
    {
        binadox::script_gen_context ctx = gen_context("default-src 'self'; img-src https: data:");
        REQUIRE(ctx.valid);
        REQUIRE(ctx.need_nonce);
    }

    TEST_CASE("github.com")
    {
        binadox::script_gen_context ctx = gen_context("default-src 'none'; base-uri 'self'; block-all-mixed-content; connect-src 'self' uploads.github.com www.githubstatus.com collector.githubapp.com api.github.com www.google-analytics.com github-cloud.s3.amazonaws.com github-production-repository-file-5c1aeb.s3.amazonaws.com github-production-upload-manifest-file-7fdce7.s3.amazonaws.com github-production-user-asset-6210df.s3.amazonaws.com cdn.optimizely.com logx.optimizely.com/v1/events wss://alive.github.com; font-src github.githubassets.com; form-action 'self' github.com gist.github.com; frame-ancestors 'none'; frame-src render.githubusercontent.com; img-src 'self' data: github.githubassets.com identicons.github.com collector.githubapp.com github-cloud.s3.amazonaws.com *.githubusercontent.com customer-stories-feed.github.com spotlights-feed.github.com; manifest-src 'self'; media-src 'none'; script-src github.githubassets.com; style-src 'unsafe-inline' github.githubassets.com; worker-src github.com/socket-worker.js gist.github.com/socket-worker.js");
        REQUIRE(ctx.valid);
        REQUIRE(ctx.need_nonce);
    }

    TEST_CASE("login.salesforce.com")
    {
        binadox::script_gen_context ctx = gen_context("upgrade-insecure-requests");
        REQUIRE(ctx.valid);
        REQUIRE_FALSE(ctx.need_nonce);
    }

    TEST_CASE("frame ancestors 1")
    {
        const std::string csp = "default-src 'self'; script-src 'self' 'nonce-wq8H6kEEqG9CZu5w0FFGIIsgUyZlcFd6' chrome-extension: 'unsafe-inline' 'unsafe-eval' *.canary.lwc.dev *.um5.visual.force.com https://ssl.gstatic.com/accessibility/ https://static.lightning.force.com https://*.salesforceliveagent.com/ 'nonce-2726c7f26c'; object-src 'self' http://c.um5.content.force.com; style-src 'self' blob: chrome-extension: 'unsafe-inline' *.um5.visual.force.com http://c.um5.content.force.com; img-src 'self' http: https: data: blob: *.um5.visual.force.com; media-src 'self' *.um5.visual.force.com http://c.um5.content.force.com https://um5.salesforce.com/content/session blob:; frame-ancestors 'self'; frame-src blob: https: mailto: *.um5.visual.force.com; font-src 'self' https: data: *.um5.visual.force.com; connect-src 'self' https://api.bluetail.salesforce.com https://staging.bluetail.salesforce.com https://preprod.bluetail.salesforce.com blob: *.um5.visual.force.com http://c.um5.content.force.com https://um5.salesforce.com";
        const std::string csp_prod = "default-src 'self'; script-src 'self' 'nonce-wq8H6kEEqG9CZu5w0FFGIIsgUyZlcFd6' chrome-extension: 'unsafe-inline' 'unsafe-eval' *.canary.lwc.dev *.um5.visual.force.com https://ssl.gstatic.com/accessibility/ https://static.lightning.force.com https://*.salesforceliveagent.com/ 'nonce-2726c7f26c'; object-src 'self' http://c.um5.content.force.com; style-src 'self' blob: chrome-extension: 'unsafe-inline' *.um5.visual.force.com http://c.um5.content.force.com; img-src 'self' http: https: data: blob: *.um5.visual.force.com; media-src 'self' *.um5.visual.force.com http://c.um5.content.force.com https://um5.salesforce.com/content/session blob:; frame-src blob: https: mailto: *.um5.visual.force.com; font-src 'self' https: data: *.um5.visual.force.com; connect-src 'self' https://api.bluetail.salesforce.com https://staging.bluetail.salesforce.com https://preprod.bluetail.salesforce.com blob: *.um5.visual.force.com http://c.um5.content.force.com https://um5.salesforce.com";
        binadox::script_gen_context ctx = gen_context(csp);
        REQUIRE(ctx.valid);
        REQUIRE(ctx.has_frame_ancestors);
        binadox::csp_header h = binadox::csp_header::parse(csp.c_str(), csp.c_str() + csp.size());
        h.remove_frame_ancestors();
        std::string prod = h.to_string();
        REQUIRE(prod == csp_prod);
    }

    TEST_CASE("frame ancestors 2")
    {
        const std::string csp = "frame-ancestors 'self';";
        const std::string csp_prod = "";
        binadox::script_gen_context ctx = gen_context(csp);
        REQUIRE(ctx.valid);
        REQUIRE(ctx.has_frame_ancestors);
        binadox::csp_header h = binadox::csp_header::parse(csp.c_str(), csp.c_str() + csp.size());
        h.remove_frame_ancestors();
        std::string prod = h.to_string();
        REQUIRE(prod == csp_prod);
    }
}
#endif