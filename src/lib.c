#include "lib.h"

Value native_print(Value v) {
    printf("%s\n", value_to_str(&v));
    return *vNull();
}

Value native_sin(Value v) {
    return *vInt((int)(100 * sin(v.value.to_int * 3.1415 / 180)));
}

void register_all_natives(program* p)
{
    create_native(p, "print", native_print);
    create_native(p, "sin", native_sin);
}