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

typedef enum asttype {
    AST_ROOT,
    AST_INTEGER,
    AST_REFERENCE,
    AST_CALL,
    AST_UNARY_EXPRESSION,
    AST_BINARY_EXPRESSION,
    AST_BLOCK
} asttype;

typedef struct astnode {
    const char* value;
    asttype type;
    vector children;
    lxpos pos;
} astnode;

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
void parsererror(parser* p, const char* msg);

/**
 * @brief Parses the full stream token into an abstract syntax tree
 *      to be interpreted.
 * 
 * @param p Reference to parser
 * @return Root node of AST
 */
astnode* parse(parser* p);

/**
 * @brief Parses expression tokens into an abstract syntax tree.
 * 
 * @param p Reference to parser
 * @return Abstract syntax tree
 */
astnode* parse_expression(parser* p);

/**
 * @brief Parses an expression primary i.e integers, variable refs,
 *      parenthesis enclosed expressions, function calls and unary
 *      expressions.
 * 
 * @param p Reference to parser
 * @return Abstract syntax tree 
 */
astnode* parse_primary(parser* p);

/**
 * @brief Parses a function call expression primary including
 *      argument expressions.
 * 
 * @param p 
 * @return astnode* 
 */
astnode* parse_function_call(parser* p);

/**
 * @brief Represents abstract syntax tree into a string representation
 *      to be printed out.
 * 
 * @param node Abstract syntax node
 * @return String
 */
const char* astnode_tostr(astnode* node);

#endif