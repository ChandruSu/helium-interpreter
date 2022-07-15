#ifndef HE_PARSER_HEADER
#define HE_PARSER_HEADER

#include "common.h"
#include "datatypes.h"
#include "lex.h"

#define PV2S(x) printf("%s\n", value_to_str(x));

typedef struct parser {
    int position;
    vector tokens;
    const char* source;
} parser;

typedef enum asttype {
    AST_ROOT,
    AST_INTEGER,
    AST_FLOAT,
    AST_BOOL,
    AST_STRING,
    AST_NULL,
    AST_REFERENCE,
    AST_CALL,
    AST_UNARY_EXPRESSION,
    AST_BINARY_EXPRESSION,
    AST_BLOCK,
    AST_ASSIGN,
    AST_FUNCTION,
    AST_PARAMS,
    AST_PARAM,
    AST_RETURN,
    AST_LOOP,
    AST_BRANCHES,
    AST_INCLUDE,
    AST_TABLE,
    AST_KV_PAIR,
    AST_PUT,
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
 * @return Root AST node
 */
astnode* parse(parser* p);

/**
 * @brief Parses multiple statements until a terminal token is hit
 *      and returns a vector node containing each statement.
 * 
 * @param p Reference to parser
 * @param terminal Terminal token
 * @return AST node
 */
astnode* parse_block(parser* p, lxtype terminal);

/**
 * @brief Parses single-line statements i.e variable assignments,
 *      function declarations, function calls etc. 
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_statement(parser* p);

/**
 * @brief Parses funcion definition into a syntax node with argument
 *      symbols and definition block.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_function_definition(parser* p);

/**
 * @brief Parses expression tokens into an abstract syntax tree.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_expression(parser* p);

/**
 * @brief Parses an expression primary i.e integers, variable refs,
 *      parenthesis enclosed expressions, function calls and unary
 *      expressions.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_primary(parser* p);

/**
 * @brief Parses a function call expression primary including
 *      argument expressions.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_function_call(parser* p);

/**
 * @brief Parses while loop control structure.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_loop(parser* p);

/**
 * @brief Parses if-else_if-else block, branching control structure.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_branching(parser* p);

/**
 * @brief Parses table definition with key value pairs.
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_table_instance(parser* p);

/**
 * @brief Parses table key-value insertion statement e.g t[k] <- v
 * 
 * @param p Reference to parser
 * @return AST node
 */
astnode* parse_table_put(parser* p);

/**
 * @brief Represents abstract syntax tree into a string representation
 *      to be printed out.
 * 
 * @param node Abstract syntax node
 * @return String
 */
const char* astnode_tostr(astnode* node);

#endif