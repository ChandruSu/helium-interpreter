#ifndef HE_PARSER_HEADER
#define HE_PARSER_HEADER

#include "common.h"
#include "datatypes.h"
#include "lex.h"

typedef struct parser {
    int position;
    vector tokens;
    const char* source;
} parser;

/**
 * @brief Returns the token at the current parser position without
 *      removing it from the token stream.
 * 
 * @param p Reference to parser
 * @return Token
 */
lxtoken* peek(parser* p);

/**
 * @brief Removes and returns the token at the current parser 
 *      position.
 * 
 * @param p Reference to parser
 * @return Token
 */
lxtoken* eat(parser* p);

/**
 * @brief Validates the current token and removes and returns it
 *      if the type matches specified; error is thrown if token
 *      cannot be validated.
 * 
 * @param p Reference to parser
 * @param type Type to validate against
 * @return Token
 */
lxtoken* consume(parser* p, lxtype type);

/**
 * @brief Removes the current parser token if it matches the type
 *      specified and returns true if matching.
 * 
 * @param p Reference to parser
 * @param type Type to check for
 * @return True if matching type
 */
boolean consume_optional(parser* p, lxtype type);

/**
 * @brief Returns true if parser has reached the end of the token
 *      stream.
 * 
 * @param p Reference to parser
 * @return True if end reached
 */
boolean is_empty(parser* p);

/**
 * @brief Terminates and prints out an error message if an invalid
 *      parser error occurs.
 * 
 * @param p Reference to parser
 * @param msg Error message to output
 */
void parser_error(parser* p, const char* msg);

#endif