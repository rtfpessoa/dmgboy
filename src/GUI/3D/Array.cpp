#include <stdlib.h>
#include <string.h>
#include "Array.h"

void *ArrayAdd(Array &a, void *data){
    if (a.elements >= a.capacity){
        a.capacity += a.delta;
        a.buffer = realloc(a.buffer, a.capacity*a.elementSize);
    }
    void *cursor = (char *)a.buffer + a.elementSize * a.elements;
    memcpy(cursor, data, a.elementSize);
    a.elements++;
    
    return cursor;
}

void ArrayClear(Array &a){
    if (a.buffer!=NULL){
        free(a.buffer);
        a.buffer = NULL;
        a.elements = 0;
        a.capacity = 0;
    }
}

unsigned long ArrayLength(const Array &a) {
    return a.elements;
}
        
