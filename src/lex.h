#ifndef HE_LEXER_HEADER
#define HE_LEXER_HEADER

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "datatypes.h"

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
    LX_LEFT_BRACE,
    LX_RIGHT_BRACE,
    LX_ASSIGN,
    LX_STRING,          // 12
    LX_FUNCTION,
    LX_CALL,
    LX_BLOCK,
    LX_SEPARATOR,       // 16
    LX_BOOL,
    LX_NULL,
    LX_RETURN,
    LX_LOOP,            // 20
    LX_IF,
    LX_ELSE,
    LX_FLOAT,
} lxtype;

typedef struct lxpos {
    int col_pos;
    int line_pos;
    int char_offset;
    int line_offset;
    const char* origin;
} lxpos;

typedef struct lxtoken {
    lxtype type;
    const char* value;
    lxpos pos;
} lxtoken;

typedef struct lexer {
    lxpos pos;
    const char* source;
    char current;
    char lookahead;
} lexer;

/**
 * @brief Lexer token constructor
 * 
 * @param value Token value
 * @param type Token type
 * @param pos Token position in source
 * @return Pointer to token
 */
lxtoken* lxtoken_new(const char* value, lxtype type, lxpos pos);

/**
 * @brief Represents lexer token as a string and prints it to
 *      standard output.
 * 
 * @param tk Token to print
 */
void lxtoken_display(lxtoken* tk);

/**
 * @brief Lexer constructor method.
 * 
 * @param src Source code string
 * @param src Origin of source code
 * @return lexer 
 */
lexer lexer_new(const char* src, const char* file_path);

/**
 * @brief Converts character buffer into a stream of lexer
 *      tokens to be parsed and interpreted.
 * 
 * @param lx Lexer state
 * @param tokens Output vector for tokens
 */
void lexify(lexer* lx, vector* tokens);

/**
 * @brief Extracts next token in source code and returns lexer
 *      token.
 * 
 * @param lx Lexer state
 * @return Pointer to token
 */
lxtoken* lex(lexer* lx);

/**
 * @brief Advances cursor by one character, updates lookahead
 *      and current members and returns the character at cursor.
 * 
 * @param lx Lexer state
 * @return Character at cursor
 */
char lexadvance(lexer* lx);

/**
 * @brief Terminates program and prints error message with the
 *      position of the error source in code and the line of code
 *      which raised the error.
 * 
 * @param lx Lexer state
 * @param msg Error message
 */
void lexerror(lexer* lx, const char* msg);

/**
 * @brief Clones lexer position and freezes it to record the locaiton
 *      of a token in source.
 * 
 * @param original Original position to be cloned
 * @return Lexer position
 */
lxpos clone_pos(lxpos* original);

#endif
