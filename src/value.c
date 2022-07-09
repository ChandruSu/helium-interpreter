#include "compiler.h"

#define TYPEPAIR(a, b) (a << 4) | b
#define TYPEMATCH(a) (a << 4) | a

const char* vm_type_strings[] = {
    "Null",
    "Int",
    "Boolean",
    "Float",
    "String",
    "Code",
};

Value value_from_node(astnode* node)
{
    Value v;
    
    switch (node->type)
    {
        case AST_INTEGER:
            v.type = VM_INT;
            v.value.to_int = atoi(node->value);
            break;
        
        case AST_FLOAT:
            v.type = VM_FLOAT;
            v.value.to_float = atof(node->value);
            break;
        
        case AST_BOOL:
            v.type = VM_BOOL;
            v.value.to_bool = streq(node->value, "true");
            break;
        
        case AST_STRING:
            v.type = VM_STRING;
            v.value.to_str = node->value;
            break;
        
        case AST_NULL:
            v.type = VM_NULL;
            v.value.to_code = NULL;
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

// --------------- Constructors ---------------

Value vNull()
{
    Value v = {};
    return v;
}

Value vInt(int i)
{
    Value v = {
        .type = VM_INT,
        .value.to_int = i
    };
    return v;
}

Value vFloat(float f)
{
    Value v = {
        .type = VM_FLOAT,
        .value.to_float = f
    };
    return v;
}

Value vString(const char* s)
{
    Value v = {
        .type = VM_STRING,
        .value.to_str = s
    };
    return v;
}

Value vBool(unsigned long b)
{
    Value v = {
        .type = VM_BOOL,
        .value.to_bool = b != 0
    };
    return v;
}

Value vCode(program* p)
{
    Value v = {
        .type = VM_PROGRAM,
        .value.to_code = p
    };
    return v;
}

// ---------------- Arithmetic ----------------

Value vAdd(Value a, Value b)
{
    char* buf;

    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool || b.value.to_bool);
        case TYPEMATCH(VM_INT): return vInt(a.value.to_int + b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vFloat(a.value.to_float + b.value.to_float);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(a.value.to_int + b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(a.value.to_float + b.value.to_int);
        case TYPEMATCH(VM_STRING):
            buf = malloc(sizeof(char) * (strlen(a.value.to_str) + strlen(b.value.to_str)));
            strcpy(buf, a.value.to_str);
            strcat(buf, b.value.to_str);
            return vString(buf);
        default:
            fprintf(stderr, "%s Cannot add values of types %s and %s\n", ERROR, vm_type_strings[a.type], vm_type_strings[b.type]);
            exit(0);
    }

    return vNull();
}
