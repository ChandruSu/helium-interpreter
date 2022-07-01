#include "datatypes.h"

// ------------------- VECTOR -------------------

vector vector_new(size_t init_capacity)
{
    vector v = {
        .items = malloc(sizeof(void*) * init_capacity),
        .size = 0,
        .capacity = init_capacity
    };
    return v;
}

void vector_resize(vector* v, size_t new_capacity)
{
    void** items = realloc(v->items, sizeof(void*) * new_capacity);

    if (items) {
        v->items = items;
        v->capacity = new_capacity;
    }
}

void vector_push(vector* v, void* item)
{
    if (v->size >= v->capacity) {
        vector_resize(v, v->capacity * 2);
    }

    v->items[v->size++] = item;
}

void* vector_pop(vector* v)
{
    return vector_rm(v, v->size - 1);
}

void* vector_top(vector* v)
{
    return v->items[v->size - 1];
}

void* vector_get(vector* v, size_t index)
{
    if (0 <= index && index < v->size) {
        return v->items[index];
    } else {
        return NULL;
    }
}

void* vector_set(vector* v, size_t index, void* item) 
{
    void* old = NULL;
 
    if (0 <= index && index < v->size) {
        old = v->items[index];
        v->items[index] = item;
    }
 
    return old;
}

void vector_insert(vector* v, size_t index, void* item)
{
    if (index < 0 || v->size <= index)
        return;

    // allocates more space if needed
    if (v->size == v->capacity)
        vector_resize(v, v->capacity * 2);

    // shifts content forwards
    for (size_t i = v->size; index < i; i--)
    {
        v->items[i] = v->items[i - 1];
        v->items[i - 1] = NULL;
    }
    
    v->items[index] = item;
    v->size++;
}

void* vector_rm(vector* v, size_t index)
{
    // handles invalid index
    if (v->size < 0 && v->size <= index) {
        return NULL;
    }

    void* item = vector_get(v, index);

    // shifts content
    for (size_t i = index; i < v->size - 1; i++)
    {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->size--;

    // resizes vector when 75% empty
    if (0 < v->size && v->size == v->capacity / 4) {
        vector_resize(v, v->capacity / 2);
    }

    return item;   
}

// --------------- TAGGED VALUES ---------------

TValue* TNull()
{
    TValue* v = (TValue*)malloc(sizeof(TValue));
    v->type = TYPE_NULL;
    v->value.s = NULL;
    return v;
}

TValue* TInt(int i)
{
    TValue* v = (TValue*)malloc(sizeof(TValue));
    v->type = TYPE_INT;
    v->value.i = i;
    return v;
}

TValue* TFloat(float f)
{
    TValue* v = (TValue*)malloc(sizeof(TValue));
    v->type = TYPE_FLOAT;
    v->value.f = f;
    return v;
}

TValue* TString(const char* s)
{
    TValue* v = (TValue*)malloc(sizeof(TValue));
    v->type = TYPE_STRING;
    v->value.s = s;
    return v;
}

TValue* TBool(boolean b)
{
    TValue* v = (TValue*)malloc(sizeof(TValue));
    v->type = TYPE_BOOLEAN;
    v->value.b = b;
    return v;
}

boolean TValEqual(TValue* a, TValue* b)
{
    if (a->type == b->type) {
        switch (a->type)
        {
            case TYPE_NULL:
                return true;
            
            case TYPE_INT:
                return a->value.i == b->value.i;

            case TYPE_FLOAT:
                return a->value.f == b->value.f;
            
            case TYPE_STRING:
                return strcmp(a->value.s, b->value.s) == 0;
                
            case TYPE_BOOLEAN:
                return a->value.b == b->value.b;
            
            default:
                return false;
        }
    } else {
        return false;
    }
}

const char* TValue_tostr(TValue* v)
{
    if (v->type == TYPE_STRING) {
        return v->value.s;
    } else {
        char* buf = (char*)malloc(sizeof(char) * 32);

        if (v->type == TYPE_BOOLEAN) {
        } else if (v->type == TYPE_INT) {
        } else if (v->type == TYPE_FLOAT) {
        } else {
        }

        switch (v->type)
        {
            case TYPE_BOOLEAN:
                sprintf(buf, "%s", v->value.b ? "True" : "False");
                break;
                
            case TYPE_INT:
                sprintf(buf, "%i", v->value.i);
                break;
                
            case TYPE_FLOAT:
                sprintf(buf, "%f", v->value.f);
                break;
                
            case TYPE_POINTER:
                sprintf(buf, "Pointer %p", v->value.pp);
                break;

            case TYPE_NULL:
                sprintf(buf, "Null");
                break;
            
            default:
                break;
        }

        return buf;
    }
}

// -------------- STRING HASH MAP --------------

map map_new(size_t init_capacity)
{
    map m = {
        .keys = (const char**) calloc(sizeof(const char*) * init_capacity, sizeof(const char*)),
        .values = (TValue**) malloc(sizeof(TValue*) * init_capacity),
        .size = 0,
        .capacity = init_capacity
    };

    return m;
}

void map_put(map* m, const char* k, TValue* v)
{
    if (((float) m->size / m->capacity) >= 0.50) {
        map_resize(m, m->capacity * 3);
    }

    size_t i = strhash(k) % m->capacity;

    for (size_t j = 0; j < m->capacity; j++)
    {
        if (m->keys[i] == NULL || strcmp(m->keys[i], k) == 0) 
        {
            m->keys[i] = k;
            m->values[i] = v;
            m->size++;
            return;
        } 
        else 
        {
            i = (i + 1) % m->capacity;
        }
    }
}

TValue* map_get(map* m, const char* k)
{
    size_t i = strhash(k) % m->capacity;

    for (size_t j = 0; j < m->capacity; j++) 
    {
        if (m->keys[i] != NULL && strcmp(m->keys[i], k) == 0) 
        {
            return m->values[i];
        } 
        else 
        {
            i = (i + 1) % m->capacity;
        }
    }
    
    return NULL;
}

const char* map_get_key(map* m, TValue* v)
{
    for (size_t i = 0; i < m->capacity; i++)
    {
        if (m->keys[i] != NULL)
        {
            if (TValEqual(m->values[i], v))
            {
                return m->keys[i];
            }
        }
    }
    return NULL;
}

TValue* map_remove(map* m, const char* k)
{
    size_t i = strhash(k) % m->capacity;

    for (size_t j = 0; j < m->capacity; j++) 
    {
        if (m->keys[i] != NULL && streq(m->keys[i], k)) {
            m->size--;
            m->keys[i] = NULL;
            return m->values[i];
        } else {
            i = (i + 1) % m->capacity;
        }
    }

    return NULL;
}

boolean map_contains(map* m, const char* k)
{
    size_t i = strhash(k) % m->capacity;

    for (size_t j = 0; j < m->capacity; j++)
    {
        if (m->keys[i] != NULL && streq(m->keys[i], k)) {
            return true;
        } else {
            i = (i + 1) % m->capacity;
        }
    }
    return false;
}

void map_resize(map* m, size_t new_capacity)
{
    map m1 = {
        .keys = (const char**) calloc(sizeof(const char*) * new_capacity, sizeof(const char*)),
        .values = (TValue**) malloc(sizeof(TValue*) * new_capacity),
        .size = 0,
        .capacity = new_capacity
    };

    for (size_t i = 0; i < m->capacity; i++)
    {
        if (m->keys[i] != NULL) 
        {
            map_put(&m1, m->keys[i], m->values[i]);
        }
    }

    free(m->keys);
    free(m->values);
    m->keys = m1.keys;
    m->values = m1.values;
    m->capacity = m1.capacity;
}
