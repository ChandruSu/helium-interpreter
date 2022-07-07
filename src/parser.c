#include "parser.h"

int precedence(parser* p, lxtoken* op);
astnode* apply_op(vector* primaries, vector* operators);
void strip_newlines(parser* p);
astnode* astnode_new(const char* value, asttype type, lxpos pos);


// ------------------ TOKEN TRAVERSAL ------------------

lxtoken* peek(parser* p)
{
    return vector_get(&p->tokens, p->position);
}

lxtoken* eat(parser* p)
{
    if (!is_empty(p)) {
        return vector_get(&p->tokens, p->position++);
    } else {
        return NULL;
    }
}

lxtoken* consume(parser* p, lxtype type)
{
    if (!is_empty(p) && peek(p)->type == type) {
        return eat(p);
    } else {
        parsererror(p, "Unexpected token");
        return NULL;
    }
}

boolean consume_optional(parser* p, lxtype type)
{
    if (!is_empty(p) && peek(p)->type == type) {
        eat(p);
        return true;
    } else {
        return false;
    }
}

boolean is_empty(parser* p)
{
    return p->position >= p->tokens.size || peek(p)->type == LX_EOF;
}

// ------------------ PARSING METHODS ------------------

astnode* parse(parser* p)
{
    astnode* tree = parse_block(p, LX_EOF);
    return tree;
}

astnode* parse_block(parser* p, lxtype terminal)
{
    astnode* block = astnode_new("block", LX_BLOCK, clone_pos(&peek(p)->pos));

    strip_newlines(p);
    
    while (!is_empty(p) && peek(p)->type != terminal) 
    {
        astnode* st = parse_statement(p);
        vector_push(&block->children, st);
        strip_newlines(p);
    }

    return block;
}

astnode* parse_statement(parser* p)
{
    astnode* st = astnode_new(NULL, AST_ASSIGN, clone_pos(&peek(p)->pos));

    switch (peek(p)->type)
    {
        case LX_SYMBOL:
            st->type = AST_ASSIGN;
            st->value = eat(p)->value;
            consume(p, LX_ASSIGN);
            
            if (peek(p)->type == LX_FUNCTION) {
                vector_push(&st->children, parse_function_definition(p));
            } else {
                vector_push(&st->children, parse_expression(p));
            }
            break;

        case LX_CALL:
            free(st);
            st = parse_function_call(p);
            break;
        
        case LX_LOOP:
            free(st);
            st = parse_loop(p);
            break;

        case LX_RETURN:
            eat(p);
            st->type = AST_RETURN;
            st->value = "ret";
            vector_push(&st->children, parse_expression(p));
            break;

        default:
            parsererror(p, "Invalid statement!");
            break;
    }

    return st;
}

astnode* parse_expression(parser* p)
{
    vector primaries = vector_new(8);
    vector operators = vector_new(8);

    vector_push(&primaries, parse_primary(p));

    // Non-primary expression
    while (!is_empty(p) && peek(p)->type == LX_OPERATOR) 
    {
        lxtoken* op = eat(p);

        // Applies shunting yard algorithm
        while (operators.size > 0 && precedence(p, op) <= precedence(p, vector_top(&operators))) 
        {
            vector_push(&primaries, apply_op(&primaries, &operators));
        }

        vector_push(&operators, op);
        vector_push(&primaries, parse_primary(p));
    }

    // Applies remaining operations
    while (operators.size > 0) 
    {
        vector_push(&primaries, apply_op(&primaries, &operators));
    }

    return vector_pop(&primaries);
}

astnode* parse_primary(parser* p)
{
    if (is_empty(p)) {
        parsererror(p, "Program has ended prematurely!");
    }

    astnode* node = (astnode*)malloc(sizeof(astnode));
    node->pos = clone_pos(&peek(p)->pos);
    node->children = vector_new(1);

    switch (peek(p)->type)
    {
        case LX_INTEGER:
            node->type = AST_INTEGER;
            node->value = eat(p)->value;
            break;
        
        case LX_BOOL:
            node->type = AST_BOOL;
            node->value = eat(p)->value;
            break;
        
        case LX_STRING:
            node->type = AST_STRING;
            node->value = eat(p)->value;
            break;
        
        case LX_SYMBOL:
            node->type = AST_REFERENCE;
            node->value = eat(p)->value;
            break;
        
        case LX_CALL:
            free(node);
            node = parse_function_call(p);
            break;

        case LX_LEFT_PAREN:
            free(node);
            consume(p, LX_LEFT_PAREN);
            node = parse_expression(p);
            consume(p, LX_RIGHT_PAREN);
            break;

        case LX_OPERATOR:
            // validates operator as unary
            if (strcmp(peek(p)->value, "-") && strcmp(peek(p)->value, "+") 
                    && strcmp(peek(p)->value, "!") && strcmp(peek(p)->value, "~")) {
                parsererror(p, "Invalid unary operator");
            }

            node->type = AST_UNARY_EXPRESSION;
            node->value = eat(p)->value;
            vector_push(&node->children, parse_primary(p));
            break;

        default:
            parsererror(p, "Unexpected token found");
    }

    return node;
}

astnode* parse_function_call(parser* p)
{
    consume(p, LX_CALL);

    astnode* fcall = (astnode*) malloc(sizeof(astnode));
    fcall->type = AST_CALL;
    fcall->pos = clone_pos(&peek(p)->pos);
    fcall->value = eat(p)->value;
    fcall->children = vector_new(4);

    consume(p, LX_LEFT_PAREN);

    if (!is_empty(p) && peek(p)->type != LX_RIGHT_PAREN) 
    {
        do 
        {
            vector_push(&fcall->children, parse_expression(p));
        } 
        while (consume_optional(p, LX_SEPARATOR));
    }

    consume(p, LX_RIGHT_PAREN);

    return fcall;
}

astnode* parse_function_definition(parser* p)
{
    astnode* func = astnode_new("code", AST_FUNCTION, clone_pos(&consume(p, LX_FUNCTION)->pos));;
    astnode* params = astnode_new("args", AST_PARAMS, clone_pos(&consume(p, LX_LEFT_PAREN)->pos));
    vector_push(&func->children, params);

    // code header
    if (peek(p)->type != LX_RIGHT_PAREN) 
    {
        do {
            lxtoken* param = consume(p, LX_SYMBOL);
            vector_push(&params->children, astnode_new(param->value, AST_PARAM, clone_pos(&param->pos)));
        } 
        while (consume_optional(p, LX_SEPARATOR));
    }
    consume(p, LX_RIGHT_PAREN);

    // code body
    consume(p, LX_LEFT_BRACE);
    vector_push(&func->children, parse_block(p, LX_RIGHT_BRACE));
    consume(p, LX_RIGHT_BRACE);

    return func;
}

astnode* parse_loop(parser* p)
{
    astnode* loop = astnode_new("loop", AST_LOOP, clone_pos(&consume(p, LX_LOOP)->pos));
    
    // loop condition
    consume(p, LX_LEFT_PAREN);
    vector_push(&loop->children, parse_expression(p));
    consume(p, LX_RIGHT_PAREN);

    // loop body
    consume(p, LX_LEFT_BRACE);
    vector_push(&loop->children, parse_block(p, LX_RIGHT_BRACE));
    consume(p, LX_RIGHT_BRACE);

    return loop;
}

// ------------------ UTILITY METHODS ------------------

astnode* astnode_new(const char* value, asttype type, lxpos pos) {
    astnode* node = (astnode*)malloc(sizeof(astnode));
    node->value = value;
    node->type = type;
    node->children = vector_new(4);
    node->pos = pos;
    return node;
}

/**
 * Returns an integer value signifying the operator order precedence for
 * the provided operator token. 
 */
int precedence(parser* p, lxtoken* op)
{
    if (streq(op->value, "<=") || streq(op->value, ">="))
        return 8;
    else if (streq(op->value, "==") || streq(op->value, "!="))
        return 7;
    else if (streq(op->value, "&&"))
        return 3;
    else if (streq(op->value, "||"))
        return 2;
    
    switch (op->value[0])
    {
        case '|':
            return 4;
        case '^':
            return 5;
        case '&':
            return 6;
        case '<':
        case '>':
            return 8;
        case '+':
        case '-':
            return 9;
        case '*':    
        case '/':    
        case '%':
            return 10;    
        default:
            parsererror(p, "Unknown operator recieved");
    }
    return 0;
}

/**
 * Applies binary operation by forming a binary Abstract syntax tree
 * with the operator as the root and the operands as the children. 
 */
astnode* apply_op(vector* operands, vector* operators)
{
    lxtoken *op, *v0, *v1;
    op = (lxtoken*) vector_pop(operators);
    v1 = (lxtoken*) vector_pop(operands);
    v0 = (lxtoken*) vector_pop(operands);

    astnode* expression = astnode_new(op->value, AST_BINARY_EXPRESSION, clone_pos(&op->pos));
    vector_push(&expression->children, v0);
    vector_push(&expression->children, v1);
    return expression;
}

void strip_newlines(parser* p)
{
    while (consume_optional(p, LX_NEWLINE));
}

const char* astnode_tostr(astnode* node)
{
    if (node->children.size == 0) {
        return node->value;
    }

    char buf[10000];
    sprintf(buf, node->type == AST_BLOCK ? "[" : "(%s", node->value);
    
    for (size_t i = 0; i < node->children.size; i++) 
    {
        astnode* child = vector_get(&node->children, i);
        sprintf(buf + strlen(buf), " %li:%s", i, astnode_tostr(child));
    }

    sprintf(buf + strlen(buf), node->type == AST_BLOCK ? "]" : ")");

    char* out = (char*)malloc(sizeof(char) * strlen(buf));
    strcpy(out, buf);
    return out;
}

void parsererror(parser* p, const char* msg) 
{
    lxtoken* tk = peek(p);
    fprintf(stderr, "%s[err] %s (%d, %d):\n", ERR_COL, msg, tk->pos.line_pos + 1, tk->pos.col_pos + 1);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", tk->pos.line_pos + 1, get_line(p->source, tk->pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + tk->pos.col_pos), DEF_COL);
    exit(0);
}