//
// Created by igor on 20/07/2020.
//

#ifndef BINADOX_ECAP_CSP_SCANNER_H
#define BINADOX_ECAP_CSP_SCANNER_H

#define CSP_TOKEN 1
#define CSP_END_OF_LIST 2

#include <stddef.h>

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
} csp_scanner;

#if defined(__cplusplus)
extern "C" {
#endif


void csp_scanner_init(csp_scanner* input, const char* text_begin, const char* text_end);
int csp_scan(csp_scanner* input);

#if defined(__cplusplus)
}
#endif
#endif //BINADOX_ECAP_CSP_SCANNER_H
