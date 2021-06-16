//
// Created by igor on 09/11/2020.
//

#include <doctest.h>
#include <string>
#include "duktape.h"

static int dummy_upper_case(duk_context *ctx) {
    size_t sz;
    const char *val = duk_require_lstring(ctx, 0, &sz);
    size_t i;

    /* We're going to need 'sz' additional entries on the stack. */
    duk_require_stack(ctx, sz);

    for (i = 0; i < sz; i++) {
        char ch = val[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - 'a' + 'A';
        }
        duk_push_lstring(ctx, (const char *) &ch, 1);
    }

    duk_concat(ctx, sz);
    return 1;
}

TEST_CASE("duktape")
{
    duk_context *ctx;



    ctx = duk_create_heap_default();
    REQUIRE(ctx != nullptr);


    duk_push_c_function(ctx, dummy_upper_case, 1);
    duk_push_string(ctx, "zopa");
    duk_call(ctx, 1);

    std::string s = duk_to_string(ctx, -1);
    REQUIRE(s == "ZOPA");

    duk_pop(ctx);

    duk_destroy_heap(ctx);
}