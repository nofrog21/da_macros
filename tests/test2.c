#include "dynamic_array.h"

#include <assert.h>
#include <stdio.h>

struct big_align {
	double d;
} __attribute__((aligned(64)));

int main() {
	struct big_align *array = NULL;
	for (size_t i = 0; i < 20; ++i) {
		struct big_align s = {.d = 123483.10293214};
		da_push(array, s);
	}
	for (size_t i = 0; i < da_len(array); ++i) {
		assert(array[i].d == 123483.10293214);
	}
	fprintf(stderr, "DEBUG: alignto struct big_align: %zu\n",
	    da_alignto(*array));
	fprintf(stderr, "DEBUG: alignof struct big_align: %zu\n",
	    alignof (*array));
	// will not overflow, alignment is bigger than sizeof
	assert(da_alignto(*array) == alignof (*array) -
	    sizeof (struct da_header));
	da_free(array);
	printf("PASSED\n");
	return 0;
}
