#include "environment.hh"

#include "api/console.hh"
#include "api/jsurl.hh"

namespace binadox
{
    namespace appgw
    {
        void setup_environment(void* duktape_ctx)
        {
            duk_context* ctx = (duk_context*)duktape_ctx;
            duk_console_init(ctx, DUK_CONSOLE_STDOUT_ONLY | DUK_CONSOLE_FLUSH);
            duk_url_init(ctx);
        }
    } // ns appgw
} // ns binadox