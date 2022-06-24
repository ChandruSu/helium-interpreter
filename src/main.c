#include "he.h"


int main(int argc, const char* argv[])
{
    const char* src;

    if (argc == 3) {
        if (strcmp(argv[1], "-f") == 0) {
            src = read_file(argv[2]);
        } else if (strcmp(argv[1], "-s") == 0) {
            src = argv[2];
        }
    } else {
        fprintf(stderr, "%s Invalid number of arguments recieved!", ERROR);
        exit(0);
    }

    // lexical analysis
    lexer lx = lexer_new(src);
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);

    printf("%s Beginning lexical anaylsis:\n\n", MESSAGE);
    
    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken_display(tokens.items[i]);
    }

    parser p = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    printf("\n%s Beginning syntax parsing:\n\n", MESSAGE);

    astnode* tree = parse(&p);

    printf("%s\n\n", astnode_tostr(tree));

    return 0;
}