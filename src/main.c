#include "he.h"


int main() 
{
    // lexical analysis
    lexer lx = lexer_new("hello(world) 1+ 2/3");
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);

    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken_display(tokens.items[i]);
    }
    
    printf("%s\n", read_file("Makefile"));

    return 0;
}