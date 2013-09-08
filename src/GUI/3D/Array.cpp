#include <stdlib.h>
#include <string.h>
#include "Array.h"

Array ArrayCreate(unsigned long elementSize, unsigned long int delta){
    Array a;
    a.buffer = NULL;
    a.elementSize = elementSize;
    a.delta = delta;
    a.elements = 0;
    a.capacity = 0;
    return a;
}    

void *ArrayAdd(Array *a, void *data){
    if (a->elements >= a->capacity){
        a->capacity += a->delta;
        a->buffer = realloc(a->buffer, a->capacity*a->elementSize);
    }
    void *cursor = (char *)a->buffer + a->elementSize * a->elements;
    memcpy(cursor, data, a->elementSize);
    a->elements++;
    
    return cursor;
}

void ArrayClear(Array *a){
    if (a->buffer!=NULL){
        free(a->buffer);
        a->buffer = NULL;
        a->elements = 0;
        a->capacity = 0;
    }
}

        
