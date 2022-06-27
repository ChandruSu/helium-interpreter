#ifndef HE_DATATYPES_HEADER
#define HE_DATATYPES_HEADER

#include "stdlib.h"
#include "common.h"


// ------------------- VECTOR -------------------

typedef struct vector {
    void** items;
    size_t size;
    size_t capacity;
} vector;

/**
 * @brief Constructor method creates empty vector with an initial
 *      capacity.
 * 
 * @param init_capacity Initial pointer storage size of vector
 * @return vector
 */
vector vector_new(size_t init_capacity);

/**
 * @brief Resizes vector and changes the memory allocated to store
 *      pointers.
 * 
 * @param v Reference to vector
 * @param new_capacity New capacity of vector
 */
void vector_resize(vector* v, size_t new_capacity);

/**
 * @brief Appends a pointer to the end of the vector and allocates
 *      more memory if needed.
 * 
 * @param v Reference to vector
 * @param item Item to append
 */
void vector_push(vector* v, void* item);

/**
 * @brief Removes and returns the final item of the vector and resizes
 *      the vector to free up memory if possible.
 * 
 * @param v Reference to vector
 * @return Pointer to removed item
 */
void* vector_pop(vector* v);

/**
 * @brief Returns the top/final pointer of the vector.
 * 
 * @param v Reference to vector
 * @return Pointer to last item
 */
void* vector_top(vector* v);

/**
 * @brief Returns an element in the vector by index.
 * 
 * @param v Reference to vector 
 * @param index Index of item
 * @return Requested item
 */
void* vector_get(vector* v, size_t index);

/**
 * @brief Overwrites a pointer in the vector at a specified
 *      position index, and returns the old occupant.
 * 
 * @param v Reference to vector
 * @param index Position of item
 * @param item New item to set
 * @return Old item
 */
void* vector_set(vector* v, size_t index, void* item) ;

/**
 * @brief Inserts an item into the vector at a specified
 *      index.
 * 
 * @param v Reference to vector
 * @param index Position the item will occupy
 * @param item Item to insert
 */
void vector_insert(vector* v, size_t index, void* item);

/**
 * @brief Removes and returns the pointer to an object at
 *      a position in the vector.
 * 
 * @param v Reference to vector
 * @param index Index of item to removes
 * @return Pointer to item
 */
void* vector_rm(vector* v, size_t index);

// ----------------- VM VALUES -----------------

typedef enum Type {
    TYPE_NULL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOLEAN,
} __attribute__ ((__packed__)) Type;

typedef union Value {
    boolean b;
    int i;
    float f;
    const char* s;
} Value;

typedef struct TValue {
    Value value;
    Type type;
} __attribute__ ((__packed__)) TValue;

TValue* TNull();

TValue* TInt(int i);

TValue* TFloat(float f);

TValue* TString(const char* s);

TValue* TBool(boolean b);

const char* TValue_tostr(TValue* v);

// -------------- STRING HASH MAP --------------

typedef struct map {
    const char** keys;
    TValue** values;
    size_t size;
    size_t capacity;
} map;

map map_new(size_t init_capacity);

void map_put(map* m, const char* k, TValue* v);

TValue* map_get(map* m, const char* k);

TValue* map_remove(map* m, const char* k);

boolean map_contains(map* m, const char* k);

void map_resize(map* m, size_t new_capacity);

#endif