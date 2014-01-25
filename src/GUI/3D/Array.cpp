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
        
void *ArrayPtr(Array &a) {
    return a.buffer;
}

const void *ArrayPtr(const Array &a) {
    return a.buffer;
}

size_t ArrayBytes(const Array &a) {
    return a.elements * a.elementSize;
}
