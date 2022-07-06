#ifndef HE_VM_HEADER
#define HE_VM_HEADER

#include <inttypes.h>
#include "compiler.h"

// --------------------- VM ---------------------

typedef struct vm {
    size_t bp;
    size_t sp;
    size_t tp;
    size_t ci;
    vector call_stack;
    Value* heap;
    Value* stack;
} vm;

#endif
