#ifndef BINADOX_ECAP_HTML_SCANNER_H
#define BINADOX_ECAP_HTML_SCANNER_H

#include <stddef.h>

#define TOKEN_HTML_SCRIPT_START 1
#define TOKEN_HTML_SCRIPT_END   2
#define TOKEN_HTML_SCRIPT_NONCE 3
#define TOKEN_HTML_HEAD_START   5
#define TOKEN_HTML_HEAD_END     6
#define TOKEN_HTML_BODY_START   7
#define TOKEN_HTML_META         8
#define TOKEN_HTML_IFRAME       9
#define TOKEN_CLOSE_TAG         10

#define HTML_SCANNER_END_OF_INPUT 0
#define HTML_SCANNER_FOUND_TOKEN  1
#define HTML_SCANNER_NOT_TOKEN    -1

typedef struct
{
    const char* tok;
    /*
     * A pointer-like l-value that stores the current input position (usually a pointer of type YYCTYPE*).
     * Initially YYCURSOR should point to the first input character.
     * It is advanced by the generated code. When a rule matches,
     * YYCURSOR points to the one after the last matched character.
     */
    const char* cursor;
    /*
     * A pointer-like l-value (usually a pointer of type YYCTYPE*) that stores the position of the latest matched rule.
     * It is used to restores YYCURSOR position if the longer match fails and lexer needs to rollback.
     * Initialization is not needed. YYMARKER is used only in the default C API.
     */
    const char* marker;
    /*
     * A pointer-like r-value that stores the end of input position (usually a pointer of type YYCTYPE*).
     * Initially YYLIMIT should point to the one after the last available input character.
     * It is not changed by the generated code. Lexer compares YYCURSOR to YYLIMIT in order to determine if there is enough input characters left.
     * YYLIMIT is used only in the default C API.
     */
    const char* limit;

    const char* token_begin;
    const char* token_end;
    int token_type;

    const char* start_of_buffer;
    size_t input_size;


    int state;
    unsigned yyaccept;

} html_scanner;

#if defined(__cplusplus)
extern "C" {
#endif


void html_scanner_init(html_scanner* input, const char* text_begin, size_t text_size);
void html_scanner_add_input(html_scanner* input, const char* text_begin, size_t delta_size);

int html_scan(html_scanner* input);

#if defined(__cplusplus)
}
#endif

#endif
