#include "lex.h"

void lexify(lexer* lx)
{
    while (lx->char_offset < strlen(lx->source)) 
    {
        printf("%c\n", lx->source[lx->char_offset]);
        ++lx->char_offset;
    }
}
