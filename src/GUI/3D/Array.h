/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct{
    void *buffer;
    unsigned long elements;
    unsigned long capacity;
    unsigned long delta;
    size_t elementSize;
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
void *ArrayPtr(Array &a);
const void *ArrayPtr(const Array &a);
template <class T>
T ArrayAt(Array &a, int index);
template <class T>
T *ArrayAtPtr(Array &a, int index);
template <class T>
const T ArrayAt(const Array &a, int index);
size_t ArrayBytes(const Array &a);

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
    return ((T *)a.buffer)[index];
}

template <class T>
T *ArrayAtPtr(Array &a, int index) {
    return &((T *)a.buffer)[index];
}

template <class T>
const T *ArrayAtPtr(const Array &a, int index) {
    return &((T *)a.buffer)[index];
}

template <class T>
const T ArrayAt(const Array &a, int index) {
    return ((T *)a.buffer)[index];
}
    
#endif
