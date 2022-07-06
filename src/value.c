#include "compiler.h"

Value value_from_node(astnode* node)
{
    Value v;
    
    switch (node->type)
    {
        case AST_INTEGER:
            v.type = VM_INT;
            v.value.to_int = atoi(node->value);
            break;
        
        // TODO: coerce floats, strings, booleans, nulls
        default: failure("Failed to coerce node to value!");
    }

    return v;
}

const char* value_to_str(Value* v)
{
    char* buf = (char*) malloc(sizeof(char) * 32);

    switch (v->type)
    {
        case VM_STRING:
            free(buf);
            return v->value.to_str;
        case VM_BOOL:
            free(buf);
            return v->value.to_bool ? "True" : "False";
        case VM_INT:
            sprintf(buf, "%i", v->value.to_int);
            return buf;
        case VM_FLOAT:
            sprintf(buf, "%f", v->value.to_float);
            return buf;
        case VM_PROGRAM:
            sprintf(buf, "<code at %p>", v->value.to_code);
            return buf;
        case VM_NULL:
            return "Null";
    }
    return NULL;
}

Value* vNull()
{
    Value* v = malloc(sizeof(Value));
    v->type = VM_NULL;
    v->value.to_code = NULL;
    return v;
}

Value* vInt(int i)
{
    Value* v = vNull();
    v->type = VM_INT;
    v->value.to_int = i;
    return v;
}

Value* vFloat(float f)
{
    Value* v = vNull();
    v->type = VM_FLOAT;
    v->value.to_float = f;
    return v;
}

Value* vString(const char* s)
{
    Value* v = vNull();
    v->type = VM_STRING;
    v->value.to_str = s;
    return v;
}

Value* vBool(boolean b)
{
    Value* v = vNull();
    v->type = VM_BOOL;
    v->value.to_bool = b;
    return v;
}

Value* vCode(program* p)
{
    Value* v = vNull();
    v->type = VM_PROGRAM;
    v->value.to_code = p;
    return v;
}