#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct{
    void *buffer;
    unsigned long elements;
    unsigned long capacity;
    unsigned long elementSize;
    unsigned long delta;
}Array;

Array ArrayCreate(unsigned long elementSize, unsigned long delta);
void *ArrayAdd(Array *a, void *data);
void  ArrayClear(Array *a);
    
#endif
