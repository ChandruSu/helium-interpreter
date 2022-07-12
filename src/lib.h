#ifndef HE_LIB_H
#define HE_LIB_H

#include "common.h"
#include "compiler.h"

#include <math.h>
#include <time.h>

/**
 * @brief Registers all native, in-built methods to the specified
 *      program's local scope. These methods will be available in
 *      the local symbol table and can be called by child methods
 * 
 * @param p Program scope
 */
void register_all_natives(program* p);

/**
 * @brief Casts generic tagged value to int-tagged value.
 * 
 * @param v Value
 * @return Int value 
 */
Value native_int_cast(Value v[]);

/**
 * @brief Casts generic tagged value to float-tagged value.
 * 
 * @param v Value
 * @return Float value 
 */
Value native_float_cast(Value v[]);

/**
 * @brief Casts generic tagged value to bool-tagged value.
 * 
 * @param v Value
 * @return Boolean value 
 */
Value native_bool_cast(Value v[]);

/**
 * @brief Casts generic tagged value to string-tagged value.
 * 
 * @param v Value
 * @return String value 
 */
Value native_str_cast(Value v[]);

#endif