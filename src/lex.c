#include "lex.h"

lxpos clone_pos(lxpos* original)
{
    lxpos pos = {
        .col_pos = original->col_pos,
        .line_pos = original->line_offset,
        .char_offset = original->char_offset,
        .line_offset = original->line_offset
    };
    return pos;
}


lxtoken* lxtoken_new(const char* value, lxtype type, lxpos pos)
{
    lxtoken* tk = (lxtoken*)malloc(sizeof(lxtoken));
    tk->pos = pos;
    tk->type = type;
    tk->value = value;
    return tk;
}

lexer lexer_new(const char* src)
{
    lexer lx = {
        .pos = {
            .col_pos = -1,
            .line_pos = 0,
            .char_offset = -1,
            .line_offset = 0
        },
        .source = src,
        .current = '\0',
        .lookahead = src[0],
    };

    return lx;
}

void lexify(lexer* lx, vector* tokens)
{
    lxtoken* token;

    while ((token = lex(lx))->type != LX_EOF) 
    {
        if (token->type != LX_WHITESPACE && token->type != LX_COMMENT) 
        {
            vector_push(tokens, token);
        }
    }
}

lxtoken* lex(lexer* lx)
{
    int len = 0;
    char* buf = (char*) malloc(sizeof(char) * 10000);

    lxtype type;
    lxpos pos = clone_pos(&lx->pos);

    if (isalpha((int) lx->lookahead)) 
    {
        type = LX_SYMBOL;
        do buf[len++] = lexadvance(lx); while (isalnum((int) lx->lookahead));
    } 
    else if (isdigit((int) lx->lookahead))
    {
        type = LX_INTEGER;
        do buf[len++] = lexadvance(lx); while (isdigit((int) lx->lookahead));
    }
    else
    {
        char c = lexadvance(lx);
        switch (c)
        {
            case '\0':
                type = LX_EOF;
                break;
            case '\n':
                type = LX_NEWLINE;
                break;
            case ' ':
            case '\r':
            case '\t':
                type = LX_WHITESPACE;
                break;
            case '+':
            case '-':
            case '/':
            case '*':
            case '%':
            case '<':
            case '>':
            case '&':
            case '|':
            case '^':
                type = LX_OPERATOR;
                buf[len++] = c;
                break;
            default:
                lexerror(lx, "Syntax error! Failed to identify symbol");
        }
    }

    // terminates string
    buf[len] = '\0';

    return lxtoken_new(buf, type, pos);
}

char lexadvance(lexer* lx)
{
    lx->current = lx->lookahead;
    lx->lookahead = lx->source[++lx->pos.char_offset + 1];

    if (lx->current == '\n') 
    {
        lx->pos.col_pos = -1;
        lx->pos.line_pos++;
        lx->pos.line_offset = lx->pos.char_offset + 1;
    } 
    else 
    {
        lx->pos.col_pos++;
    }

    return lx->current;
}

void lexerror(lexer* lx, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d) %s\n", ERR_COL, msg, lx->pos.line_pos, lx->pos.col_pos, DEF_COL);
    exit(0);
}