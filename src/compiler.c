#include "compiler.h"

vm_op decode_binary_op(const char* operator);
vm_op decode_unary_op(const char* operator);
void runtimeerr(virtual_machine* vm, const char* msg);

vm_op scope_load_op_map[] = {
    OP_LOADL,
    OP_LOADG,
    OP_LOADC,
    OP_NOP,
};

vm_op scope_store_op_map[] = {
    OP_STORL,
    OP_STORG,
    OP_STORC,
    OP_NOP,
};

// -------------- COMPILER METHODS --------------

void compile(program* p, astnode* block)
{
    for (size_t i = 0; i < block->children.size; i++)
    {
        compile_statement(p, vector_get(&block->children, i));
    }
}

void compile_statement(program* p, astnode* statement)
{
    recordaddress(p, &statement->pos);

    switch (statement->type)
    {
        case AST_ASSIGN:
            compile_assignment(p, statement);
            break;
        
        case AST_CALL:
            compile_call(p, statement);
            p->code[p->length++].stackop.op = OP_POP;
            break;
        
        case AST_RETURN:
            if (p->prev == NULL) {
                compilererr(p, statement->pos, "Cannot use return statement in global scope!");
            }

            compile_expression(p, vector_get(&statement->children, 0));
            p->code[p->length++].stackop.op = OP_RET;
            break;

        case AST_INCLUDE:
            astnode* fp = vector_get(&statement->children, 0);
            run_import(p, fp);
            break;

        case AST_LOOP:
            compile_loop(p, statement);
            break;

        case AST_BRANCHES:
            compile_branches(p, statement);
            break;

        case AST_PUT:
            compile_table_put(p, statement);
            break;
        
        case AST_GET:
            compile_table_get(p, statement);
            break;

        default:
            compilererr(p, statement->pos, "Failed to compile statement into bytecode!");
    }
}

void compile_assignment(program* p, astnode* s)
{
    vm_scope scope;
    astnode* rhs = vector_get(&s->children, 0);
    int16_t address = register_variable(p, s->value, &scope);

    if (address >= MAX_LOCAL_VARIABLES) {
        compilererr(p, s->pos, "Maxmum variables in local scope achieved!");
    }

    compile_expression(p, rhs);
    p->code[p->length].sx.sx = address;
    p->code[p->length].sx.op = scope_store_op_map[scope];
    p->length++;
}

void compile_call(program* p, astnode* call)
{
    for (size_t i = 1; i < call->children.size; i++)
    {
        compile_expression(p, vector_get(&call->children, i));   
    }
    
    // vm_scope scope;
    // p->code[p->length].sx.sx = dereference_variable(p, call->value, &scope);
    // p->code[p->length].sx.op = scope_load_op_map[scope];
    // p->length++;

    // if (scope == VM_UNKNOWN_SCOPE) {
    //     compilererr(p, call->pos, "Unknown function name!");
    // }

    compile_expression(p, vector_rm(&call->children, 0));

    p->code[p->length].ux.op = OP_CALL;
    p->code[p->length].ux.ux = call->children.size;
    p->length++;
}

void compile_function(program* p, astnode* function)
{
    program* p0 = malloc(sizeof(program));
    p0->code = malloc(sizeof(instruction) * 0xff);
    p0->length = 0;
    p0->constants = malloc(sizeof(Value) * 0xff);
    p0->prev = p;
    p0->constant_table = map_new(37);
    p0->symbol_table = map_new(37);
    p0->line_address_table = map_new(37);
    p0->native = NULL;

    // register parameter names
    astnode* params = vector_get(&function->children, 0);
    for (p0->argc = 0; p0->argc < params->children.size; p0->argc++)
    {
        vm_scope scope;
        astnode* param = vector_get(&params->children, p0->argc);
        register_unique_variable_local(p0, param->value, &scope);  
    
        if (scope == VM_DUPLICATE_IN_SCOPE) {
            compilererr(p0, param->pos, "Duplicate variable name in function definition!");
        }
    }

    // compiles program code
    compile(p0, vector_get(&function->children, 1));;

    if (p0->code[p0->length-1].stackop.op != OP_RET) {
        p0->code[p0->length].ux.op = OP_PUSHK;
        p0->code[p0->length++].ux.ux = register_constant(p0, vNull());
        p0->code[p0->length++].stackop.op = OP_RET;
    }

    // stores code object as local constant
    p->code[p->length].ux.op = OP_PUSHK;
    p->code[p->length].ux.ux = register_constant(p, vCode(p0, NULL));
    p->length++;

    if (p0->closure_table.size) {

        // loads closure values to create closure object
        for (size_t i = 0; i < p0->closure_table.size; i++) {
            vm_scope scope;
            p->code[p->length].sx.sx = dereference_variable(p, p0->closure_table.keys[i], &scope);
            p->code[p->length].sx.op = scope_load_op_map[scope];
            p->length++;
        }
        
        p->code[p->length].ux.op = OP_CLOSE;
        p->code[p->length].ux.ux = p0->closure_table.size;
        p->length++;
    }
}

void compile_expression(program* p, astnode* expression)
{
    recordaddress(p, &expression->pos);

    switch (expression->type)
    {
        case AST_BINARY_EXPRESSION:
            compile_expression(p, vector_get(&expression->children, 0));
            compile_expression(p, vector_get(&expression->children, 1));
            p->code[p->length++].stackop.op = decode_binary_op(expression->value);
            break;
        
        case AST_UNARY_EXPRESSION:
            compile_expression(p, vector_get(&expression->children, 0));
            p->code[p->length++].stackop.op = decode_unary_op(expression->value);
            break;

        case AST_CALL:
            compile_call(p, expression);
            break;
        
        case AST_FUNCTION:
            compile_function(p, expression);
            break;

        case AST_REFERENCE:
            vm_scope scope;
            p->code[p->length].sx.sx = dereference_variable(p, expression->value, &scope);
            p->code[p->length].sx.op = scope_load_op_map[scope];
            p->length++;

            if (scope == VM_UNKNOWN_SCOPE)
                compilererr(p, expression->pos, "Unknown variable name!");
            break;
        
        case AST_TABLE:
            compile_table(p, expression);
            break;
        
        case AST_GET:
            compile_table_get(p, expression);
            break;

        case AST_INTEGER:
        case AST_FLOAT:
        case AST_STRING:
        case AST_BOOL:
        case AST_NULL:
            p->code[p->length].ux.op = OP_PUSHK;
            p->code[p->length].ux.ux = register_constant(p, value_from_node(expression));
            p->length++;
            break;
        
        default:
            compilererr(p, expression->pos, "Failed to compile expression!");
    }
}

void compile_loop(program* p, astnode* loop)
{
    int pos0 = p->length;

    compile_expression(p, vector_get(&loop->children, 0));
    p->code[p->length++].stackop.op = OP_JIF;

    int pos1 = p->length++;
    
    compile(p, vector_get(&loop->children, 1));

    // restart loop
    p->code[p->length].sx.op = OP_JMP;
    p->code[p->length].sx.sx = pos0 - p->length - 1;
    p->length++;

    // jump to end
    p->code[pos1].sx.op = OP_JMP;
    p->code[pos1].sx.sx = p->length - pos1 - 1;
}

void compile_branches(program* p, astnode* branches)
{
    // compile condition
    compile_expression(p, vector_get(&branches->children, 0));
    p->code[p->length++].stackop.op = OP_JIF;
    int pos0 = p->length++;

    // compile body
    compile(p, vector_get(&branches->children, 1));
    int pos1 = p->length++;

    // skip body if condition not met
    p->code[pos0].sx.op = OP_JMP;
    p->code[pos0].sx.sx = p->length - pos0 - 1;

    astnode* alt = vector_get(&branches->children, 2);

    if (alt != NULL) 
    {
        if (streq(alt->value, "conditional")) {
            compile_branches(p, alt);
        } else {
            compile(p, vector_get(&alt->children, 0));
        }

        p->code[pos1].sx.op = OP_JMP;
        p->code[pos1].sx.sx = p->length - pos1 - 1;
    } 
    else 
    {
        p->code[pos1].stackop.op = OP_NOP;
    }
}

void compile_table(program* p, astnode* table)
{
    p->code[p->length++].stackop.op = OP_TNEW;

    for (size_t i = 0; i < table->children.size; i++)
    {
        astnode* pair = vector_get(&table->children, i);
        compile_expression(p, pair->children.items[0]); 
        compile_expression(p, pair->children.items[1]);
        p->code[p->length++].stackop.op = OP_TPUT;
    }
}

void compile_table_put(program* p, astnode* put) 
{
    vm_scope scope;
    p->code[p->length].sx.sx = dereference_variable(p, put->value, &scope);
    p->code[p->length].sx.op = scope_load_op_map[scope];
    p->length++;

    compile_expression(p, vector_get(&put->children, 0));
    compile_expression(p, vector_get(&put->children, 1));
    p->code[p->length++].stackop.op = OP_TPUT;
}

void compile_table_get(program* p, astnode* get)
{
    vm_scope scope;
    p->code[p->length].sx.sx = dereference_variable(p, get->value, &scope);
    p->code[p->length].sx.op = scope_load_op_map[scope];
    p->length++;

    compile_expression(p, vector_get(&get->children, 0));
    p->code[p->length++].stackop.op = OP_TGET;
}

void create_native(program* p, const char* name, Value (*f)(Value[]), int argc)
{
    program* p0 = (program*) malloc(sizeof(program));
    p0->code = NULL;
    p0->length = 0;
    p0->argc = argc;
    p0->constants = NULL;
    p0->symbol_table = map_new(0);
    p0->constant_table = map_new(0);
    p0->closure_table = map_new(0);
    p0->line_address_table = map_new(0);
    p0->prev = p;
    p0->native = f;

    p->code[p->length].ux.op = OP_PUSHK;
    p->code[p->length].ux.ux = register_constant(p, vCode(p0, NULL));
    p->length++;

    vm_scope scope;
    uint16_t address = register_variable(p, name, &scope);
    
    if (address >= MAX_LOCAL_VARIABLES) {
        lxpos pos = { .col_pos = 0, .line_pos = 0, .line_offset = 0, .char_offset = 0};
        compilererr(p, pos, "Maxmum variables in local scope achieved!");
    }

    // stores code at address
    p->code[p->length].sx.sx = address;
    p->code[p->length].sx.op = scope_store_op_map[scope];
    p->length++;
}

void run_import(program* p, astnode* filepath)
{
    if (p->prev != NULL) {
        compilererr(p, filepath->pos, "Cannot import in local scope!");
    }

    // determines absolute system path of include
    char* path = (char*)malloc(sizeof(char) * 512);
    path[0] = '\0';
    strcpy(path, filepath->pos.origin);
    dirname(path);
    strcat(path, "/");
    strcat(path, filepath->value);

    const char* src = read_file(path);

    vector tokens = vector_new(64);
    lexer lx = lexer_new(src, path);
    lexify(&lx, &tokens);

    parser p0 = {
        .position = 0,
        .source = src,
        .tokens = tokens
    };

    astnode* tree = parse(&p0);
    
    compile(p, tree);
}

// ---------------- MEMORY STORE ----------------

uint16_t register_constant(program* p, Value v)
{
    Value* address;

    if ((address = map_get(&p->constant_table, value_to_str(&v))) == NULL) 
    {
        address = malloc(sizeof(Value));
        *address = vInt(p->constant_table.size);

        if (address->value.to_int >= MAX_LOCAL_CONSTANTS) {
            failure("Max constants in local scope reached!");
        }

        map_put(&p->constant_table, value_to_str(&v), address);
        p->constants[address->value.to_int] = v;
    }
    
    return address->value.to_int;
}

int16_t register_variable(program* p, const char* name, vm_scope* scope)
{
    size_t address = dereference_variable(p, name, scope);

    if (*scope == VM_UNKNOWN_SCOPE) {
        Value* a = malloc(sizeof(Value)); 
        *a = vInt(p->symbol_table.size);
        map_put(&p->symbol_table, name, a);
        *scope = p->prev == NULL ? VM_GLOBAL_SCOPE : VM_LOCAL_SCOPE;
        return a->value.to_int;
    } else {
        return address;
    }
}

// TODO: change this, poorly implemented
int16_t register_unique_variable_local(program* p, const char* name, vm_scope* scope)
{
    Value* address = map_get(&p->symbol_table, name);

    if (address == NULL) {
        address = malloc(sizeof(Value));
        *address = vInt(p->symbol_table.size);
        map_put(&p->symbol_table, name, address);
        *scope = p->prev == NULL ? VM_GLOBAL_SCOPE : VM_LOCAL_SCOPE;
        return address->value.to_int;
    } else {
        *scope = VM_DUPLICATE_IN_SCOPE;
        return address->value.to_int;
    }
}

int16_t dereference_variable(program* p, const char* name, vm_scope* scope)
{
    Value* address = map_get(&p->symbol_table, name);
    program* p0 = p;

    // iteratively determines variable location
    while (address == NULL && p0->prev != NULL) {
        p0 = p0->prev;
        address = map_get(&p0->symbol_table, name);
    }

    // determines scope of variable
    if (address == NULL) {
        *scope = VM_UNKNOWN_SCOPE;
        return 0;
    } else if (p0->prev == NULL) {
        *scope = VM_GLOBAL_SCOPE;
    } else if (p0 == p) {
        *scope = VM_LOCAL_SCOPE;
    } else {
        *scope = VM_CLOSED_SCOPE;

        // retrieves or registers closure address
        if (map_has(&p->closure_table, name)) {
            address = map_get(&p->closure_table, name);
        } else {
            Value* a = malloc(sizeof(Value)); 
            *a = vInt(p->closure_table.size);
            map_put(&p->closure_table, name, a);
            address = a;
        }
    }
    
    return address->value.to_int;
}

// ------------------- UTILS --------------------

vm_op decode_binary_op(const char* operator)
{
    if (streq(operator, "+")) // arithmetic
        return OP_ADD;
    else if (streq(operator, "-"))
        return OP_SUB;
    else if (streq(operator, "*"))
        return OP_MUL;
    else if (streq(operator, "/"))
        return OP_DIV;
    else if (streq(operator, "%"))
        return OP_MOD;
    else if (streq(operator, "&&")) // boolean
        return OP_AND;
    else if (streq(operator, "||"))
        return OP_OR;
    else if (streq(operator, "=="))
        return OP_EQ;
    else if (streq(operator, "!="))
        return OP_NE;
    else if (streq(operator, "<"))
        return OP_LT;
    else if (streq(operator, "<="))
        return OP_LE;
    else if (streq(operator, ">"))
        return OP_GT;
    else if (streq(operator, ">="))
        return OP_GE;
    else {
        failure("Failed to decode binary operator!");
    }
    return OP_NOP;
}

vm_op decode_unary_op(const char* operator)
{
    if (streq(operator, "+"))
        return OP_NOP;
    else if (streq(operator, "-"))
        return OP_NEG;
    else if (streq(operator, "!"))
        return OP_NOT;
    else
        failure("Failed to decode unary operator!");
    return OP_NOP;
}

#ifdef HE_DEBUG_MODE
const char* operation_strings[] = {
    "OP_NOP      ",
    "OP_ADD      ",
    "OP_SUB      ",
    "OP_MUL      ",
    "OP_DIV      ",
    "OP_MOD      ",
    "OP_NEG      ",
    "OP_NOT      ",
    "OP_AND      ",
    "OP_OR       ",
    "OP_EQ       ",
    "OP_NE       ",
    "OP_LT       ",
    "OP_LE       ",
    "OP_GT       ",
    "OP_GE       ",
    "OP_PUSHK    ",
    "OP_STORG    ",
    "OP_LOADG    ",
    "OP_STORL    ",
    "OP_LOADL    ",
    "OP_STORC    ",
    "OP_LOADC    ",
    "OP_CALL     ",
    "OP_RET      ",
    "OP_POP      ",
    "OP_JIF      ",
    "OP_JMP      ",
    "OP_CLOSE    ",
    "OP_TNEW     ",
    "OP_TPUT     ",
    "OP_TGET     ",
    "OP_TREM     ",
};

const char* disassemble_program(program* p) 
{
    char* buf = malloc(sizeof(char) * 32 * 100);
    buf[0] = '\0';

    // disassembles instructions
    for (size_t i = 0; i < p->length; i++)
    {
        char b[5];
        sprintf(b, "%03li", i);
        strcat(buf, b);
        strcat(buf, "\t");
        strcat(buf, disassemble(p, p->code[i]));
        strcat(buf, "\n");
    }

    for (size_t i = 0; i < p->constant_table.size; i++)
    {
        Value* index = p->constant_table.values[i];
        Value program = p->constants[index->value.to_int];

        if (program.type == VM_PROGRAM && program.value.to_code->p->native == NULL)
        {
            strcat(buf, "\n");
            strcat(buf, value_to_str(&program));
            strcat(buf, ":\n");
            strcat(buf, disassemble_program(p->constants[index->value.to_int].value.to_code->p));
        }
    }
    
    return buf;
}

const char* disassemble(program* p, instruction i) {
    char* buf = malloc(sizeof(char) * 32);

    switch (i.stackop.op)
    {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_NEG:
        case OP_NOT:
        case OP_AND:
        case OP_OR:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_RET:
        case OP_POP:
        case OP_NOP:
        case OP_JIF:
        case OP_TNEW:
        case OP_TPUT:
        case OP_TGET:
        case OP_TREM:
            sprintf(buf, "%s", operation_strings[i.stackop.op]);
            break;
        
        case OP_CALL:
        case OP_CLOSE:
            sprintf(buf, "%s %u", operation_strings[i.stackop.op], i.ux.ux);
            break;
        
        case OP_JMP:
            sprintf(buf, "%s %i", operation_strings[i.stackop.op], i.sx.sx);
            break;
        
        case OP_PUSHK:
            Value k = p->constants[i.ux.ux];
            sprintf(buf, "%s %u (%s)", operation_strings[i.stackop.op], i.ux.ux, value_to_str(&k));
            break;
        
        case OP_LOADC:
        case OP_STORC:
            const char* c = p->closure_table.keys[i.ux.ux];
            sprintf(buf, "%s %u (%s)", operation_strings[i.stackop.op], i.ux.ux, c);
            break;

        case OP_STORG:
        case OP_LOADG:
            // uses global program to decode symbols
            while (p->prev != NULL) p = p->prev;
        
        case OP_STORL:
        case OP_LOADL:
            const char* vname;

            // decodes reference name in local symbol table
            for (size_t i0 = 0; i0 < p->symbol_table.size; i0++) {
                if (((Value*)p->symbol_table.values[i0])->value.to_int == i.sx.sx) {
                    vname = p->symbol_table.keys[i0];
                    break;
                }
            }
            
            sprintf(buf, "%s %i (%s)", operation_strings[i.sx.op], i.sx.sx, vname);
            break;

        default:
            failure("Failed to disassemble instruction!");
    }
    return buf;
}
#endif

void compilererr(program* p, lxpos pos, const char* msg)
{
    fprintf(stderr, "%s[err] %s (%d, %d) in %s:\n", ERR_COL, msg, pos.line_pos + 1, pos.col_pos + 1, pos.origin);
    fprintf(stderr, "\t|\n");
    fprintf(stderr, "\t| %04i %s\n", pos.line_pos + 1, get_line(pos.src, pos.line_offset));
    fprintf(stderr, "\t| %s'\n%s", paddchar('~', 5 + pos.col_pos), DEF_COL);
    exit(0);
}

void recordaddress(program* p, lxpos* pos)
{
    lxpos* last = p->line_address_table.values[p->line_address_table.size - 1];

    if (p->line_address_table.size == 0 || last->line_pos < pos->line_pos) {
        char* buf = malloc(sizeof(char) * 8);
        sprintf(buf, "%li", p->length);

        map_put(&p->line_address_table, buf, pos);
    }
}

lxpos* getaddresspos(program* p, int pos)
{
    for (size_t i = p->line_address_table.size - 1; i >= 0; i--)
    {
        size_t pos0 = atoi(p->line_address_table.keys[i]);

        if (pos0 <= pos) {
            return p->line_address_table.values[i];
        }
    }
    return NULL;
}