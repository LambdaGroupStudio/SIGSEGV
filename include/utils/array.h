#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Dynamic array utility (like std::vector in C++)
//
// header only so all API is decl'd as static.
//
// some things to note: all internal decls are
// denoted by __

typedef size_t index_t; /* index into the array    */
typedef size_t count_t; /* number of elements      */
typedef size_t bytes_t; /* number of bytes         */

/*
    elem_size  = size in bytes of a single element
    size       = number of valid elements
    capacity   = number of allocated slots (in elements)
*/
typedef struct
{
  void*       arr_data;
  count_t size;
  count_t capacity;
  bytes_t elem_size;
} __DynArray;

// it is syntactically easier to understand __arr(T) instead of __DynArray
//
// in reality, __DynArray is a generic dynamic array that can hold any type of data.
// this declaration is just for syntactic sugar.
//
// always prefer using dyn_arr(T).
#define dyn_arr(T) __DynArray

// create array with no allocation
inline static __DynArray dyn_arr_create(bytes_t elem_size)
{
  __DynArray a;
  a.arr_data  = NULL;
  a.size      = 0;
  a.capacity  = 0;
  a.elem_size = elem_size;
  return a;
}

// free array
inline static void dyn_arr_free(__DynArray* a)
{
  free(a->arr_data);
  a->arr_data  = NULL;
  a->size      = 0;
  a->capacity  = 0;
  a->elem_size = 0;
}

// get the element count in array
inline static count_t dyn_arr_size(const __DynArray* a) { return a->size; }

// Ensure capacity for new_cap elements
inline static void dyn_arr_reserve(__DynArray* a, count_t new_cap)
{
  if (new_cap > a->capacity)
  {
    void* p     = realloc(a->arr_data, (bytes_t)new_cap * a->elem_size);
    a->arr_data = p;
    a->capacity = new_cap;
  }
}

// Push element to end
inline static void dyn_arr_push_back(__DynArray* a, const void* elem)
{
  if (a->size == a->capacity)
    dyn_arr_reserve(a, a->capacity ? a->capacity * 2 : 8);

  memcpy((char*)a->arr_data + a->size * a->elem_size, elem, a->elem_size);

  a->size++;
}

// Pop element from end
inline static void dyn_arr_pop_back(__DynArray* a, void* out_elem)
{
  if (a->size == 0)
    return;

  a->size--;

  if (out_elem)
    memcpy(out_elem, (char*)a->arr_data + a->size * a->elem_size, a->elem_size);
}

// Get element pointer (mutable)
inline static void* dyn_arr_get(__DynArray* a, index_t idx)
{
  if (idx >= a->size)
    return NULL;
  return (char*)a->arr_data + idx * a->elem_size;
}

// Get element pointer (const)
inline static const void* dyn_arr_get_const(const __DynArray* a, index_t idx)
{
  if (idx >= a->size)
    return NULL;
  return (const char*)a->arr_data + idx * a->elem_size;
}

// Get pointer to the entire array data (read-only)
inline static const void* dyn_arr_get_data_const(const __DynArray* a)
{
  if (!a || !a->arr_data || a->size == 0)
    return NULL;
  return a->arr_data;
}

// Get typed pointer to entire array (read/write) - so not recommended to use this but well it exists
#define __arr_get_data(a, T) ((T*)((a) && (a)->arr_data ? (a)->arr_data : NULL))

// Set element at a particular index in array
inline static void dyn_arr_set(__DynArray* a, index_t idx, const void* elem)
{
  if (idx >= a->size)
    return;

  memcpy((char*)a->arr_data + idx * a->elem_size, elem, a->elem_size);
}

// Check whether initialized + contains elements
inline static bool dyn_arr_exists(const __DynArray* array)
{
  return array && array->arr_data != NULL && array->size > 0;
}

// push element at a particular index
//
// just involves pushing all elements from that index onwards
// to the right then placing the new element
inline static void dyn_arr_push_at(__DynArray* a, index_t idx, const void* elem)
{
    if (!a || !elem)
        return;

    if (idx > a->size)
        idx = a->size;

    // check if cap is valid
    if (a->size == a->capacity)
        dyn_arr_reserve(a, a->capacity ? a->capacity * 2 : 8);

    memmove((char*)a->arr_data + (idx + 1) * a->elem_size,
        (char*)a->arr_data + idx * a->elem_size,
        (a->size - idx) * a->elem_size
    );

    memcpy((char*)a->arr_data + idx * a->elem_size,elem,a->elem_size);

    a->size++;
}

// pop element at a particular index
inline static void dyn_arr_pop_at(__DynArray* a, index_t idx)
{
    if (!a || idx >= a->size)
        return;

    if (idx != a->size - 1)
        dyn_arr_set(a, idx, dyn_arr_get(a, a->size - 1));

    a->size--;
}
