#include "lib.h"

Value native_print(Value v[]) 
{
    printf("%s\n", value_to_str(&v[0]));
    return vNull();
}

Value native_input(Value v[]) 
{
    int ch, extra;

    if (v->value.to_str != NULL) {
        printf ("%s", v->value.to_str);
        fflush (stdout);
    }

    char* buf = malloc(sizeof(char) * 1000);
    
    if (fgets(buf, 1000, stdin) == NULL) {
        return vNull();
    }
    
    if (buf[strlen(buf)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        if (extra) vNull();
    }

    buf[strlen(buf)-1] = '\0';
    return vString(buf);
}

Value native_int_cast(Value v[]) 
{
    switch (v[0].type)
    {
        case VM_INT: return v[0];
        case VM_FLOAT: return vInt((int)v[0].value.to_float);
        case VM_STRING: return vInt(atoi(v[0].value.to_str));
        case VM_BOOL: return vInt(v[0].value.to_bool);
        case VM_NULL: return vInt(0);
        case VM_PROGRAM: return vInt(0);
    }
    return vNull();
}

Value native_float_cast(Value v[]) 
{
    switch (v[0].type)
    {
        case VM_INT: return vFloat((float)v[0].value.to_int);
        case VM_FLOAT: return v[0];
        case VM_STRING: return vFloat(atof(v[0].value.to_str));
        case VM_BOOL: return vFloat((float)v[0].value.to_bool);
        case VM_NULL: return vFloat(0);
        case VM_PROGRAM: return vFloat(0);
    }
    return vNull();
}

Value native_bool_cast(Value v[]) 
{
    switch (v[0].type)
    {
        case VM_INT: return vBool(v[0].value.to_int != 0);
        case VM_FLOAT: return vBool(v[0].value.to_float != 0);
        case VM_STRING: return vBool(strlen(v[0].value.to_str));
        case VM_BOOL: return v[0];
        case VM_NULL: return vBool(0);
        case VM_PROGRAM: return vBool(0);
    }
    return vNull();
}

Value native_str_cast(Value v[]) 
{
    return vString(value_to_str(&v[0]));
}

Value native_length(Value v[])
{
    switch (v[0].type)
    {
        case VM_INT: return v[0];
        case VM_FLOAT: return vInt((int) v[0].value.to_float);
        case VM_STRING: return vInt(strlen(v[0].value.to_str));
        case VM_BOOL: return v[0];
        case VM_NULL: return vInt(0);
        case VM_PROGRAM: return vInt(v[0].value.to_code->p->argc);
    }
    return vNull();
}

Value native_sqrt(Value v[])
{
    if (v[0].type == VM_FLOAT) 
        return vFloat(sqrt(v[0].value.to_float));
    else 
        return vFloat(sqrt(v[0].value.to_int));
}

Value native_pow(Value v[])
{   
    switch (TYPEPAIR(v[0].type, v[1].type))
    {
        case TYPEMATCH(VM_INT): return vInt(powl(v[0].value.to_int, v[1].value.to_int));
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(powf((float) v[0].value.to_int, v[1].value.to_float));
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(powf(v[0].value.to_float, (float) v[1].value.to_int));
        case TYPEMATCH(VM_FLOAT): return vFloat(powf(v[0].value.to_float, v[1].value.to_float));
    
        default: return vNull();
    }
}

void register_all_natives(program* p)
{
    create_native(p, "print", native_print, 1);
    create_native(p, "input", native_input, 1);
    create_native(p, "int", native_int_cast, 1);
    create_native(p, "str", native_str_cast, 1);
    create_native(p, "float", native_float_cast, 1);
    create_native(p, "bool", native_bool_cast, 1);
    create_native(p, "len", native_length, 1);
    create_native(p, "sqrt", native_sqrt, 1);
    create_native(p, "pow", native_pow, 2);
}