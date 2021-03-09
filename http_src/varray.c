/*
 * vararray.c
 *
 * Functions that implement a generic variable length array.
 *
 */
#include "varray.h"

#include <strings.h>
#include <stdio.h>

/** Generic variable length array */
struct VArray {
	void* bytes;  		/** array bytes */
	size_t size;		/** number of array elements */
	size_t width;		/** width of array element */
	size_t capacity;	/** capacity of array */
};

/**
 * Ensure varray has required capacity. If not, expand
 * varray to closest power of 2 for specified capacity.
 *
 * @param varray the varray
 * @param capacity the required capacity
 * @return true if can expand varray, false if cannot expand
 */
static bool ensureCapacity(VArray* varray, size_t capacity) {
	if (varray->capacity < capacity) { // current capacity too small
		// ensure capacity is the power of 2 greater or equal to specified capacity
		capacity = (capacity == 0) ? 1 : 1 << flsl(capacity-1);

		// realloc bytes for capacity elements of element width
		void* bytes = realloc(varray->bytes, capacity*varray->width);
		if (bytes == NULL) {  // out of memory
			return false;
		}

		// update bytes and capacity
		varray->bytes = bytes;
		varray->capacity = capacity;
	}

	return true;
}

/**
 * Create VArray with elements of width and initial capacity.
 *
 * @param width width of element
 * @param capacity intial capacity of array
 * @return the new instance or NULL if no space
 */
void* newVArray(size_t width, size_t capacity) {
	// allocate instance
	VArray* varray = malloc(sizeof(VArray));
	if (varray == NULL) {
		return NULL;
	}

	// initialize with element width (other fields 0/NULL)
	*varray = (VArray){.width = width};

	// ensure initial capacity is a power of 2, and at least 4
	if (!ensureCapacity(varray, (capacity < 4) ? 4 : capacity)) {
		// out of space
		deleteVArray(varray);
		return NULL;
	}

	return varray;
}

/**
 * Delete a VArray.
 *
 * @param varray the varray
 */
void deleteVArray(VArray* varray) {
	// free bytes of varray
	free(varray->bytes);

	// reset fields of varray
	*varray = (VArray){};

	// free varray
	free(varray);
}

/**
 * Gets pointer to array element at specified index.
 * Extends varray if necessary to contain index.
 *
 * @param varray the varray
 * @param index the index
 * @return pointer to element or NULL if cannot expand varray
 */
void* elementAtVArray(VArray* varray, size_t index) {
	// ensure capacity is large enough to contain index
	if (!ensureCapacity(varray, index+1)) {
		return NULL; // cannot expand capacity
	}

	// ensure size is large enough to contain index
	if (varray->size <= index) {
		varray->size = index+1;    // extend size of varray to index
	}

	// point to byte offset of index element
	return (varray->bytes) + index*(varray->width);
}

/**
 * Gets number of elements in varray.
 *
 * @param varray the varray
 * @return number of elements in varray
 */
size_t sizeVArray(VArray* varray) {
	return varray->size;
}

/**
 * Gets number of elements available in varray.
 *
 * @param varray the varray
 * @return number of elements available in varray
 */
size_t capacityVArray(VArray* varray) {
	return varray->capacity;
}
