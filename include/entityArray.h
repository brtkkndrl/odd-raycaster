#ifndef ENTITY_ARRAY_H
#define ENTITY_ARRAY_H

#include <stdlib.h>

typedef struct EntityArray
{
    size_t size;
    size_t capacity;
    size_t elemSize;
    char *arr;
} EntityArray;

void EntityArray_init(EntityArray *arr, size_t capacity, size_t elemSize);

void EntityArray_free(EntityArray *arr);

void EntityArray_add(EntityArray *arr, void* element);

void EntityArray_remove(EntityArray *arr, size_t index);

void* EntityArray_get(EntityArray* arr, size_t index);

#endif