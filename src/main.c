#include "he.h"


int main(int argc, const char* argv[])
{
    const char* src;

    if (argc == 3)
    {
        if (strcmp(argv[1], "-f") == 0)
        {
            src = read_file(argv[2]);
        } 
        else if (strcmp(argv[1], "-s") == 0)
        {
            src = argv[2];
        }
    } 
    else 
    {
        fprintf(stderr, "%s Invalid number of arguments recieved!", ERROR);
        exit(0);
    }

    // lexical analysis
    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);

    lexer lx = lexer_new(src);
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);
    
    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken_display(tokens.items[i]);
    }

    // abstract syntax parsing
    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p);
    printf("%s\n\n", astnode_tostr(tree));

    // interpreting
    interpreter in = {
        .root = tree
    };

    printf("\n%s Beginning interpretting:\n\n", MESSAGE);

    interpret(&in);

    return 0;
}