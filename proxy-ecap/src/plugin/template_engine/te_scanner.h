#ifndef BINADOX_ECAP_TE_SCANNER_H
#define BINADOX_ECAP_TE_SCANNER_H

#include <stddef.h>

#define TE_TOKEN_END 0
#define TE_TOKEN_TEXT 1
#define TE_TOKEN_VARIABLE 2

typedef struct
{

    const char* top;
    const char* cur;
    const char* ptr;
    const char* pos;
    const char* end;

    const char* token_begin;
    const char* token_end;
    int token_type;
} te_scanner;

#if defined(__cplusplus)
extern "C" {
#endif


void te_scanner_init(te_scanner* input, const char* text_begin, const char* text_end);
int te_scan(te_scanner* input);

#if defined(__cplusplus)
}
#endif

#endif
