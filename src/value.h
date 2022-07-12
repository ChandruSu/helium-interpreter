#ifndef HE_VALUE_HEADER
#define HE_VALUE_HEADER

#include "common.h"
#include "parser.h"

#define TYPEPAIR(a, b) (a << 4) | b
#define TYPEMATCH(a) (a << 4) | a

// ------------ Forward Declarations ------------

typedef struct program program;
typedef struct Value Value;
typedef struct virtual_machine virtual_machine;

// Globally accessible virtual machine instance
extern virtual_machine* current_vm;

// ------------------ VM TYPES ------------------

typedef struct code_object {
    program* p;
    Value* closure;
} code_object;

typedef enum vm_type {
    VM_NULL,
    VM_INT,
    VM_BOOL,
    VM_FLOAT,
    VM_STRING,
    VM_PROGRAM,
} vm_type;

typedef struct Value {
    vm_type type;
    union {
        boolean to_bool;
        int32_t to_int;
        float to_float;
        const char* to_str;
        code_object* to_code;
    } value;
} Value;

/**
 * @brief Coerces abstract syntax node into Value object.
 * 
 * @param node Abstract syntax node
 * @return Value
 */
Value value_from_node(astnode* node);

/**
 * @brief Represents VM Value object as a string.
 * 
 * @param v Value
 * @return string
 */
const char* value_to_str(Value* v);

/**
 * @brief Constructor for null value.
 * 
 * @return Value
 */
Value vNull();

/**
 * @brief Constructor for int value.
 * 
 * @return Value
 */
Value vInt(int i);

/**
 * @brief Constructor for float value.
 * 
 * @return Value
 */
Value vFloat(float f);

/**
 * @brief Constructor for string value.
 * 
 * @return Value
 */
Value vString(const char* s);

/**
 * @brief Constructor for bool value.
 * 
 * @return Value
 */
Value vBool(unsigned long b);

/**
 * @brief Constructor for code object value.
 * 
 * @param p Reference to program
 * @param closure Closure object containing constants
 * 
 * @return Value
 */
Value vCode(program* p, Value* closure);

/**
 * @brief Performs addition between two Value operands.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Result value
 */
Value vAdd(Value a, Value b);

/**
 * @brief Performs subtraction between two Value operands.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Result value
 */
Value vSub(Value a, Value b);

/**
 * @brief Performs multiplication between two Value operands.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Result value
 */
Value vMul(Value a, Value b);

/**
 * @brief Returns the negated form of a generic tagged value.
 * 
 * @param a Operand
 * @return Result value 
 */
Value vNegate(Value a);

/**
 * @brief Performs division between two Value operands.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Result value
 */
Value vDiv(Value a, Value b);

/**
 * @brief Performs the modulus operation between two Value operands.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Result value
 */
Value vMod(Value a, Value b);

/**
 * @brief Compares two generic values and returns boolean value True
 *      if they are equal.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Boolean result value
 */
Value vEqual(Value a, Value b);

/**
 * @brief Compares two generic values and returns boolean value False
 *      if they are equal.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Boolean result value
 */
Value vNotEqual(Value a, Value b);

/**
 * @brief Performs less-than operation between two generic tagged values
 *      and returns true if the first is less than the second.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Boolean result value
 */
Value vLess(Value a, Value b);

/**
 * @brief Performs less-than or equal to operation between two generic 
 *      tagged values and returns true if the first is less than the 
 *      second or equal to the second.
 * 
 * @param a Operand 1
 * @param b Operand 2
 * @return Boolean result value
 */
Value vLessEqual(Value a, Value b);

#endif