#include "he.h"


int main(int argc, const char* argv[])
{
    const char* src;

    if (argc < 3) {
        fprintf(stderr, "%s Invalid number of arguments recieved!", ERROR);
        exit(0);
    }

    printf("\n%s Reading code:\n\n%s\n", MESSAGE, src);

    // lexical analysis
    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);

    lexer lx = lexer_new(src);
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);
    
    for (size_t i = 0; i < tokens.size; i++)
        lxtoken_display(tokens.items[i]);

    // syntax parsing
    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p);
    printf("%s\n", astnode_tostr(tree));

    printf("\n%s Beginning compilation:\n\n", MESSAGE);

    
    compile();
    
    printf("\n%s Program has ended successfully!\n\n", MESSAGE);
    return 0;
}