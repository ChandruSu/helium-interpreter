
#include "he.h"

int main() 
{
    // lexical analysis
    lexer lx = lexer_new("1 + 2");
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);

    return 0;
}