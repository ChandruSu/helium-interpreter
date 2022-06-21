
#include "he.h"

int main() 
{
    lexer lx = {
        .col_pos = 0,
        .line_pos = 0,
        .char_offset = 0,
        .source = "1 + 2",
    };

    lexify(&lx);
}