#ifndef HE_LEXER_HEADER
#define HE_LEXER_HEADER

// includes
#include "common.h"
#include "ctype.h"
#include "datatypes.h"
#include "string.h"
#include "stdio.h"


typedef enum lxtype {
    LX_SYMBOL,          // 0
    LX_INTEGER,
    LX_OPERATOR,
    LX_EOF,
    LX_COMMENT,         // 4
    LX_NEWLINE,
    LX_WHITESPACE,
    LX_LEFT_PAREN,
    LX_RIGHT_PAREN,     // 8
} lxtype;

typedef struct filepos {
    int col_pos;
    int line_pos;
    int char_offset;
    int line_offset;
} filepos;

filepos clone_pos(filepos* original);

typedef struct lxtoken {
    lxtype type;
    const char* value;
    filepos pos;
} lxtoken;

typedef struct lexer {
    filepos pos;
    const char* source;
    char current;
    char lookahead;
} lexer;

lxtoken* lxtoken_new(lexer* lx, const char* value, lxtype type);

lexer lexer_new(const char* src);

/**
 * @brief Converts character buffer into a stream of lexer
 *      tokens to be parsed and interpreted.
 * 
 * @param lx Lexer state
 * @param tokens Output vector for tokens
 */
void lexify(lexer* lx, vector* tokens);

lxtoken* lex(lexer* lx);

char lexadvance(lexer* lx);

void lexerror(lexer* lx, const char* msg);

#endif
