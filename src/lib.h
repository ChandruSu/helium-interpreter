#ifndef HE_LIB_H
#define HE_LIB_H

#include "common.h"
#include "compiler.h"

#include <math.h>

/**
 * @brief Registers all native, in-built methods to the specified
 *      program's local scope. These methods will be available in
 *      the local symbol table and can be called by child methods
 * 
 * @param p Program scope
 */
void register_all_natives(program* p);

#endif