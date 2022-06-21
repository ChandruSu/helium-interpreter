#ifndef HE_LEXER_HEADER
#define HE_LEXER_HEADER

// includes
#include "string.h"
#include "stdio.h"


typedef enum lxtype {
    LX_WHITESPACE
} lxtype;


typedef struct lexer {
    int col_pos;
    int line_pos;
    int char_offset;
    const char* source;
} lexer;


typedef struct lxtoken {
    lxtype type;
    const char* value;
} lxtoken;

/**
 * @brief Converts character buffer into a stream of lexer
 *      tokens to be parsed and interpreted.
 * 
 * @param lx Lexer state
 */
void lexify(lexer* lx);

#endif
