#include "lib.h"

Value native_print(Value v) {
    printf("%s\n", value_to_str(&v));
    return vNull();
}

Value native_input(Value v) {
    printf("%s", value_to_str(&v));
    char* buffer = malloc(sizeof(char) * 1000);
    fscanf(stdin, "%s", buffer);
    return vString(buffer);
}

Value native_int_cast(Value v) {
    switch (v.type)
    {
        case VM_INT: return v;
        case VM_FLOAT: return vInt((int)v.value.to_float);
        case VM_STRING: return vInt(atoi(v.value.to_str));
        case VM_BOOL: return vInt(v.value.to_bool);
        case VM_NULL: return vInt(0);
        case VM_PROGRAM: return vInt(0);
    }
    return vNull();
}

void register_all_natives(program* p)
{
    create_native(p, "print", native_print);
    create_native(p, "input", native_input);
    create_native(p, "int", native_int_cast);
}