#include "parser.h"


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
        // throw error
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
    return p->position >= p->tokens.size;
}

void parser_error(parser* p, const char* msg)
{
    lxtoken* tk = peek(p);
    fprintf(stderr, "%s[err] Parser error! %s%s\n", ERR_COL, msg, DEF_COL);
}