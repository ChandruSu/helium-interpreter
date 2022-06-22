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

const char* lxtype_strings[] = {
    "LX_SYMBOL      ",
    "LX_INTEGER     ",
    "LX_OPERATOR    ",
    "LX_EOF         ",
    "LX_COMMENT     ",
    "LX_NEWLINE     ",
    "LX_WHITESPACE  ",
    "LX_LEFT_PAREN  ",
    "LX_RIGHT_PAREN ",
    "LX_ASSIGN      ",
    "LX_STRING      ",
};

lxtoken* lxtoken_new(const char* value, lxtype type, lxpos pos)
{
    lxtoken* tk = (lxtoken*)malloc(sizeof(lxtoken));
    tk->pos = pos;
    tk->type = type;
    tk->value = value;
    return tk;
}

void lxtoken_display(lxtoken* tk)
{
    printf("(%03i, %03i) %s %s\n", tk->pos.line_pos, tk->pos.col_pos, lxtype_strings[tk->type], tk->value);   
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

    // Advancced position
    lxpos pos = clone_pos(&lx->pos);
    pos.col_pos++;
    pos.char_offset++;

    if (isalpha((int) lx->lookahead) || lx->lookahead == '_')
    {
        type = LX_SYMBOL;
        do buf[len++] = lexadvance(lx); while (isalnum((int) lx->lookahead) || isdigit((int) lx->lookahead) || lx->lookahead == '_');
    } 
    else if (isdigit((int) lx->lookahead))
    {
        type = LX_INTEGER;
        do buf[len++] = lexadvance(lx); while (isdigit((int) lx->lookahead));
    }
    else if (lx->lookahead == '"')
    {
        char c =lexadvance(lx);
        while ((c = lexadvance(lx)) != '"') buf[len++] = c;
        type = LX_STRING;
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
            case ':':
                type = LX_ASSIGN;
                break;
            case '+': // numeric operations
            case '-':
            case '/':
            case '*':
            case '%':
            case '&': // bitwise operations
            case '|':
            case '^':
            case '~':
                type = LX_OPERATOR;
                buf[len++] = c;
                break;
            case '(':
                type = LX_LEFT_PAREN;
                break;
            case ')':
                type = LX_RIGHT_PAREN;
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
    fprintf(stderr, "%s[err] %s (%d, %d)\n%s\n", ERR_COL, msg, lx->pos.line_pos + 1, lx->pos.col_pos + 1, DEF_COL);
    exit(0);
}