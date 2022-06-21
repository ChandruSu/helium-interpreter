
#include "he.h"

int main() 
{
    lexer lx = lexer_new("1 + 2");
    vector tokens = vector_new(10);
    lexify(&lx, &tokens);

    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken* tk = tokens.items[i];
        printf("%s %i\n", tk->value, tk->type);
    }

    return 0;
}