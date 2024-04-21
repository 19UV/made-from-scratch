#ifndef INCLUDED_UTIL_VECTOR_H
#define INCLUDED_UTIL_VECTOR_H

#include <stddef.h>

/* TODO: Like C++ std::vector allocate in larger chunks.
         Avoid realloc every append.
*/

typedef struct {
	void* data;

	size_t length;

	size_t element_size;
} Vector;

#define VECTOR( T) create_vector(sizeof( T))

Vector create_vector(size_t element_size);
void destroy_vector(Vector* vector);

void* vector_append(Vector* vector, const void* element);

#endif /* INCLUDED_UTIL_VECTOR_H */
