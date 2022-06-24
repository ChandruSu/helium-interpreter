#include "he.h"


int main(int argc, const char* argv[])
{
    // file path
    const char* file_name;
    if (argc > 1) {
        file_name = argv[1];
    }

    // lexical analysis
    lexer lx = lexer_new(read_file(file_name));
    vector tokens = vector_new(16);
    lexify(&lx, &tokens);

    printf("%s Beginning lexical anaylsis:\n", MESSAGE);
    
    for (size_t i = 0; i < tokens.size; i++)
    {
        lxtoken_display(tokens.items[i]);
    }

    return 0;
}