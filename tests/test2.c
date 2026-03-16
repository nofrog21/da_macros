#include "dynamic_array.h"
#undef DA_DEFAULT_CAPACITY
#define DA_DEFAULT_CAPACITY 1

#include <assert.h>
#include <stdio.h>

struct big_align {
	double d;
} __attribute__((aligned(256)));

int main() {
	struct big_align *array = NULL;
	struct da_header *hdr = NULL;
	for (size_t i = 0; i < 20; ++i) {
		struct big_align s = {.d = 123483.10293214};
		da_push(array, s);
		hdr = da_unsafe_get_header(array);
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
