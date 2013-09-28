#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct{
    void *buffer;
    unsigned long elements;
    unsigned long capacity;
    unsigned long elementSize;
    unsigned long delta;
}Array;

template <class T>
Array ArrayCreate(int increment=50);
void *ArrayAdd(Array &a, void *data);
void  ArrayClear(Array &a);
unsigned long ArrayLength(const Array &a);
template <class T>
T *ArrayPtr(Array &a);
template <class T>
const T *ArrayPtr(const Array &a);
template <class T>
T ArrayAt(Array &a, int index);

//----------------------------------------

template <class T>
Array ArrayCreate(int increment) {
    Array a;
    a.buffer = NULL;
    a.elementSize = sizeof(T);
    a.delta = increment;
    a.elements = 0;
    a.capacity = 0;
    return a;
}

template <class T>
T *ArrayPtr(Array &a) {
    return (T *)a.buffer;
}

template <class T>
const T *ArrayPtr(const Array &a) {
    return (T *)a.buffer;
}

template <class T>
T ArrayAt(Array &a, int index) {
    return *((T *)a.buffer)[index];
}

    
#endif
