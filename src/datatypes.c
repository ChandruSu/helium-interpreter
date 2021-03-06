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

void vector_delete(vector* v)
{
    free(v->items);
    free(v);
}

// ----------------- STRING MAP -----------------

map map_new(size_t init_capacity)
{
    map m = {
        .keys = malloc(sizeof(const char*) * init_capacity),
        .values = malloc(sizeof(void*) * init_capacity),
        .size = 0,
        .capacity = init_capacity
    };
    return m;
}

void* map_get(map* m, const char* key)
{
    for (size_t i = 0; i < m->size; i++) {
        if (streq(key, m->keys[i])) {
            return m->values[i];
        }
    }
    return NULL;
}

void map_put(map* m, const char* key, void* value)
{
    for (size_t i = 0; i < m->size; i++) {
        if (streq(key, m->keys[i])) {
            m->values[i] = value;
            return;
        }
    }

    if (m->size == m->capacity) {
        map_resize(m, m->capacity * 2);
    }

    m->keys[m->size] = key;
    m->values[m->size++] = value;
}

void* map_rm(map* m, const char* key)
{
    void* out = NULL;
    for (size_t i = 0; i < m->size; i++) {
        if (streq(key, m->keys[i])) {
            out = m->values[i];
            m->size--;
        }
        if (out) {
            m->keys[i] = m->keys[i + 1];
            m->values[i] = m->values[i + 1];
        }
    }

    m->keys[m->size] = NULL;
    m->values[m->size] = NULL;

    if (m->size < m->capacity / 4) {
        map_resize(m, m->capacity / 2);
    }

    return out;
}

boolean map_has(map* m, const char* key)
{
    for (size_t i = 0; i < m->size; i++) {
        if (streq(key, m->keys[i])) {
            return true;
        }
    }
    return false;
}

void map_resize(map* m, size_t new_capacity)
{
    const char** keys = realloc(m->keys, sizeof(const char*) * new_capacity);
    void** values = realloc(m->values, sizeof(void*) * new_capacity);

    if (keys && values) {
        m->keys = keys;
        m->values = values;
        m->capacity = new_capacity;
    }
}

void map_delete(map* m)
{
    free(m->keys);
    free(m->values);
    free(m);
}