#include "util/vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

Vector create_vector(size_t element_size) {
	assert(element_size > 0);

	return (Vector) {
		.data = NULL,
		.length = 0,
		.element_size = element_size
	};
}

void destroy_vector(Vector* vector) {
	assert(vector != NULL);

	if (vector->length > 0) {
		assert(vector->data != NULL);

		free(vector->data);

#ifndef NDEBUG
		vector->data = 0;
#endif
	} else {
		assert(vector->data == NULL);
	}
}

void* vector_append(Vector* vector, const void* element) {
	assert(vector != NULL);

	assert(
		vector->length == 0
			? vector->data == NULL
			: vector->data != NULL
	);

	const size_t old_length = vector->length++;

	void* new_data = realloc(vector->data, vector->length * vector->element_size);
	if (new_data == NULL) {
		fputs("[ERROR] Out of Memory.\n", stderr);

		vector->length = old_length;

		return NULL;
	}

	void* appended = (char*)vector->data + old_length * vector->element_size;

	if (element != NULL) {
		memcpy(appended, element, vector->element_size);
	}

	return appended;
}
