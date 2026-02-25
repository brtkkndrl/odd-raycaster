#include "entityArray.h"

#include <string.h>

void EntityArray_init(EntityArray *arr, size_t capacity, size_t elemSize){
    arr->capacity = capacity;
    arr->size = 0;
    arr->elemSize = elemSize;
    arr->arr = calloc(capacity, elemSize);
    if (arr->arr == NULL)//TODO
        exit(1);
}

void EntityArray_free(EntityArray *arr){
    free(arr->arr);
}

void EntityArray_add(EntityArray *arr, void* element){
    if(arr->size == arr->capacity){
        exit(2);
    }
    memcpy(arr->arr + arr->size*arr->elemSize, element, arr->elemSize);
    arr->size++;
}

void EntityArray_remove(EntityArray *arr, size_t index){
    if (index >= arr->size){//TODO
        exit(2);
    }

    memmove(arr->arr + index*arr->elemSize, arr->arr + (index+1)*arr->elemSize, (arr->size-1-index)*arr->elemSize);

    arr->size--;
}

void* EntityArray_get(EntityArray* arr, size_t index){
    if(index >= arr->size){//TODO
        exit(2);
    }
    return (void*)arr->arr + index*arr->elemSize;
}