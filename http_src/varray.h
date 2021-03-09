/*
 * vararray.h
 *
 * Functions that implement a generic variable length array.
 *
 */

#ifndef VARRAY_H_
#define VARRAY_H_
#include <stdbool.h>
#include <stdlib.h>

/** Generic variable length array */
typedef struct VArray VArray;

/**
 * Create new VArray of elements of width and initial capacity.
 *
 * @param width width of element
 * @param capacity intial capacity of array
 * @return the new instance
 */
void* newVArray(size_t width, size_t capacity);

/**
 * Delete a VArray.
 *
 * @param varray the varray
 */
void deleteVArray(VArray* varray);

/**
 * Gets pointer to array element at specified index.
 * Extends varray if necessary to contain index.
 *
 * @param varray the varray
 * @param index the index
 * @return pointer to element or NULL if cannot expand varray
 */
void* elementAtVArray(VArray* varray, size_t index);

/**
 * Gets number of elements in varray.
 *
 * @param varray the varray
 * @return number of elements in varray
 */
size_t sizeVArray(VArray* varray);

/**
 * Gets number of elements available in varray.
 *
 * @param varray the varray
 * @return number of elements available in varray
 */
size_t capacityVArray(VArray* varray);

#endif /* VARRAY_H_ */
