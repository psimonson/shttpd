/*
 * vector.h - Simple vector implementation.
 *
 * Author: Philip R. Simonson
 * Date  : 11/11/2021
 *
 *****************************************************************************
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* For use with this file DO NOT use in code.
 */
#define vector_set_size(vec, size) \
	do { \
		if((vec)) { \
			((size_t*)(vec))[-1] = (size); \
		} \
	} while(0)

/* DO NOT use for this file.
 */
#define vector_size(vec) ((vec) ? ((size_t*)(vec))[-1] : 0)

/* Get the number of elements in the array.
 */
#define vector_count(vec) ((vec) ? ((size_t*)(vec))[-2] : 0)

/* Is the array empty?
 */
#define vector_empty(vec) (vector_count((vec)) == 0)

/* Return the first element in the given vector.
 */
#define vector_beg(vec) ((vec) ? &(vec)[0] : NULL)

/* Return the last element in the given vector.
 */
#define vector_end(vec) ((vec) ? &(vec)[vector_count((vec))] : NULL)

/* DO NOT use: Call vector_add() instead.
 */
#define vector_grow(vec, size) \
	do { \
		const size_t newsz = (size) * sizeof(*(vec)) * (sizeof(size_t) * 2); \
		if(!(vec)) { \
			size_t *p = malloc(newsz); \
			assert(p); \
			(vec) = (void*)(&p[2]); \
			vector_set_size((vec), (size)); \
			((size_t*)(vec))[-2] = 0; \
		} \
		else { \
			size_t *p1 = &((size_t *)(vec))[-2]; \
			size_t *p2 = realloc(p1, newsz); \
			assert(p2); \
			(vec) = (void*)(&p2[2]); \
			vector_set_size((vec), (size)); \
		} \
	} while(0)

/* Find an element inside of the array.
 */
#define vector_find(vec, item, type, exists) \
	do { \
		if((vec)) { \
			type p = vector_beg((vec)); \
			while(p != vector_end((vec)) && *p != (item)) p++; \
			*(exists) = (*p == (item) ? 1 : 0); \
		} \
		else { \
			*(exists) = 0; \
		} \
	} while(0)

/* Add an element into the array.
 */
#define vector_add(vec, item) \
	do { \
		size_t _cap = vector_size((vec)); \
		if(_cap <= vector_count((vec))) { \
			vector_grow((vec), _cap + 1); \
		} \
		(vec)[vector_count((vec))] = (item); \
		((size_t*)(vec))[-2]++; \
	} while(0)

/* Free the vector.
 */
#define vector_free(vec) \
	do { \
		if((vec)) { \
			void *p = &((size_t*)(vec))[-2]; \
			free(p); \
		} \
	} while(0)

#endif
