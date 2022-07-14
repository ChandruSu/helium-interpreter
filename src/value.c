#include "value.h"

void runtimeerr(virtual_machine* vm, const char* msg);

const char* vm_type_strings[] = {
    "Null",
    "Int",
    "Boolean",
    "Float",
    "String",
    "Code",
    "Table",
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
    char* buf = (char*) malloc(sizeof(char) * 64);

    switch (v->type)
    {
        case VM_STRING:
            return v->value.to_str;
        case VM_BOOL:
            free(buf);
            return v->value.to_bool ? "true" : "false";
        case VM_INT:
            sprintf(buf, "%i", v->value.to_int);
            return buf;
        case VM_FLOAT:
            sprintf(buf, "%f", v->value.to_float);
            return buf;
        case VM_PROGRAM:
            sprintf(buf, "<code at %p>", v->value.to_code->p);
            return buf;
        case VM_NULL:
            return "null";
        case VM_TABLE:
            sprintf(buf, "<table at %p>", v->value.to_table);
            return buf;
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

Value vCode(program* p, Value* closure)
{
    Value v = {
        .type = VM_PROGRAM,
        .value.to_code = malloc(sizeof(code_object))
    };
    v.value.to_code->p = p;
    v.value.to_code->closure = closure;
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
        case TYPEPAIR(VM_INT, VM_BOOL): return vInt(a.value.to_int + b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vInt(a.value.to_bool + b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(a.value.to_int + b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(a.value.to_float + b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vFloat(a.value.to_float + b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vFloat(a.value.to_bool + b.value.to_float);
        case TYPEMATCH(VM_STRING):
            buf = malloc(sizeof(char) * (strlen(a.value.to_str) + strlen(b.value.to_str) + 1));
            strcpy(buf, a.value.to_str);
            strcat(buf, b.value.to_str);
            buf[strlen(buf)] = '\0';
            return vString(buf);
        default:
            char buf0[100];
            sprintf(buf0, "Cannot add values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf0);
    }

    return vNull();
}

Value vSub(Value a, Value b)
{
    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool - b.value.to_bool);
        case TYPEMATCH(VM_INT): return vInt(a.value.to_int - b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vFloat(a.value.to_float - b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vInt(a.value.to_int - b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vInt(a.value.to_bool - b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(a.value.to_int - b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(a.value.to_float - b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vFloat(a.value.to_float - b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vFloat(a.value.to_bool - b.value.to_float);
        default:
            char buf[100];
            sprintf(buf, "Cannot subtract values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vMul(Value a, Value b)
{
    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool && b.value.to_bool);
        case TYPEMATCH(VM_INT): return vInt(a.value.to_int * b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vFloat(a.value.to_float * b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vInt(a.value.to_int * b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vInt(a.value.to_bool * b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(a.value.to_int * b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(a.value.to_float * b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vFloat(a.value.to_float * b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vFloat(a.value.to_bool * b.value.to_float);
        default:
            char buf[100];
            sprintf(buf, "Cannot multiply values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vDiv(Value a, Value b)
{
    char buf[100];
    
    if (b.value.to_int == 0 || b.value.to_float == 0) {
        sprintf(buf, "Zero division error!");
        runtimeerr(current_vm, buf);
    }

    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool / b.value.to_bool);
        case TYPEMATCH(VM_INT): return vInt(a.value.to_int / b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vFloat(a.value.to_float / b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vInt(a.value.to_int / b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vInt(a.value.to_bool / b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vFloat(a.value.to_int / b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vFloat(a.value.to_float / b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vFloat(a.value.to_float / b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vFloat(a.value.to_bool / b.value.to_float);
        default:
            sprintf(buf, "Cannot divide values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vMod(Value a, Value b)
{
    char buf[100];

    if (a.type != VM_STRING && a.type != VM_TABLE && b.value.to_int == 0) {
        sprintf(buf, "Zero modulus error!");
        runtimeerr(current_vm, buf);
    }

    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool % b.value.to_bool);
        case TYPEMATCH(VM_INT): return vInt(a.value.to_int % b.value.to_int);
        case TYPEPAIR(VM_INT, VM_BOOL): return vInt(a.value.to_int % b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vInt(a.value.to_bool % b.value.to_int);
        case TYPEPAIR(VM_STRING, VM_INT):
            if (b.value.to_int < 0 || b.value.to_int >= strlen(a.value.to_str)) {
                sprintf(buf, "String index [%i] out of bounds!", b.value.to_int);
                runtimeerr(current_vm, buf);
            }

            char* c = malloc(sizeof(char) * 2);
            c[0] = a.value.to_str[b.value.to_int];
            c[1] = '\0';
            return vString(c);
        case TYPEPAIR(VM_TABLE, VM_INT):
            if (b.value.to_int < 0 || b.value.to_int >= a.value.to_table->size) {
                sprintf(buf, "Table index [%i] out of bounds!", b.value.to_int);
                runtimeerr(current_vm, buf);
            }
            return a.value.to_table->pairs[b.value.to_int].key;
        default:
            sprintf(buf, "Cannot apply modulo values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vEqual(Value a, Value b)
{
    if (a.type == VM_NULL || b.type == VM_NULL) {
        return vBool(a.type == VM_NULL && b.type == VM_NULL);
    }

    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_STRING): return vBool(streq(a.value.to_str, b.value.to_str));
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool == b.value.to_bool);
        case TYPEMATCH(VM_INT): return vBool(a.value.to_int == b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vBool(a.value.to_float == b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vBool(a.value.to_int == b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vBool(a.value.to_bool == b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vBool(a.value.to_int == b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vBool(a.value.to_float == b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vBool(a.value.to_float == b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vBool(a.value.to_bool == b.value.to_float);
        default:
            return vBool(false);
    }

    return vNull();
}

Value vNotEqual(Value a, Value b)
{
    return vBool(!vEqual(a,b).value.to_bool);
}

Value vNegate(Value a)
{
    switch (a.type)
    {
        case VM_BOOL: return vBool(1 - a.value.to_bool);
        case VM_INT: return vInt(-a.value.to_int);
        case VM_FLOAT: return vFloat(-a.value.to_float);
        default:
            char buf[100];
            sprintf(buf, "Cannot negate value of type %s!", vm_type_strings[a.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vLess(Value a, Value b)
{
    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool < b.value.to_bool);
        case TYPEMATCH(VM_INT): return vBool(a.value.to_int < b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vBool(a.value.to_float < b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vBool(a.value.to_int < b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vBool(a.value.to_bool < b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vBool(a.value.to_int < b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vBool(a.value.to_float < b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vBool(a.value.to_float < b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vBool(a.value.to_bool < b.value.to_float);
        default:
            char buf[100];
            sprintf(buf, "Cannot perform that operation between values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

Value vLessEqual(Value a, Value b)
{
    switch (TYPEPAIR(a.type, b.type))
    {
        case TYPEMATCH(VM_BOOL): return vBool(a.value.to_bool <= b.value.to_bool);
        case TYPEMATCH(VM_INT): return vBool(a.value.to_int <= b.value.to_int);
        case TYPEMATCH(VM_FLOAT): return vBool(a.value.to_float <= b.value.to_float);
        case TYPEPAIR(VM_INT, VM_BOOL): return vBool(a.value.to_int <= b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_INT): return vBool(a.value.to_bool <= b.value.to_int);
        case TYPEPAIR(VM_INT, VM_FLOAT): return vBool(a.value.to_int <= b.value.to_float);
        case TYPEPAIR(VM_FLOAT, VM_INT): return vBool(a.value.to_float <= b.value.to_int);
        case TYPEPAIR(VM_FLOAT, VM_BOOL): return vBool(a.value.to_float <= b.value.to_bool);
        case TYPEPAIR(VM_BOOL, VM_FLOAT): return vBool(a.value.to_bool <= b.value.to_float);
        default:
            char buf[100];
            sprintf(buf, "Cannot perform that operation between values of types %s and %s!", vm_type_strings[a.type], vm_type_strings[b.type]);
            runtimeerr(current_vm, buf);
    }

    return vNull();
}

// ------------ TABLE DATA STRUCTURE ------------

Value vTable(size_t init_capacity)
{
    Value v = {
        .type = VM_TABLE,
        .value.to_table = NULL
    };

    v.value.to_table = malloc(sizeof(Table));
    v.value.to_table->capacity = init_capacity;
    v.value.to_table->size = 0;
    v.value.to_table->pairs = calloc(init_capacity, 2 * sizeof(Value));

    return v;
}

void _vTable_resize(Table* t, size_t new_capacity)
{
    t->pairs = realloc(t->pairs, 2 * sizeof(Value) * new_capacity);
    
    if (t->pairs) {
        t->capacity = new_capacity;
    } else {
        runtimeerr(current_vm, "Failed to resize table!");
    }
}

Value vTableGet(Table* t, Value k)
{
    for (size_t i = 0; i < t->size; i++)
    {
        if (vEqual(t->pairs[i].key, k).value.to_bool) {
            return t->pairs[i].value;
        }
    }
    return vNull();
}

void vTablePut(Table* t, Value k, Value v)
{
    for (size_t i = 0; i < t->size; i++) 
    {
        if (vEqual(t->pairs[i].key, k).value.to_bool) {
            t->pairs[i].value = v;
            return;
        }
    }

    if (t->size == t->capacity) {
        _vTable_resize(t, t->capacity * 2);
    }

    t->pairs[t->size].key = k;
    t->pairs[t->size].value = v;
    t->size++;
    return;
}

Value vTableRm(Table* t, Value k)
{
    size_t i;
    Value out = vNull();

    for (i = 0; i < t->size; i++)
    {
        if (vEqual(t->pairs[i].key, k).value.to_bool) {
            out = t->pairs[i].value;
            t->size--;
            break;
        }
    }

    for (; i < t->size; i++)
    {
        t->pairs[i].key = t->pairs[i + 1].key;
        t->pairs[i].value = t->pairs[i + 1].value;
    }
    
    t->pairs[t->size].key = vNull();
    t->pairs[t->size].value = vNull();
    
    if (t->size < t->capacity / 4) {
        _vTable_resize(t, t->capacity / 2);
    }

    return out;
}

void vTableDelete(Table* t) 
{
    free(t->pairs);
}