#include "dynamic_array.h"

#include <assert.h>
#include <stdio.h>

int main() {
	int *array = NULL;
	int err = 0;
	for (int i = 0; i <= DA_DEFAULT_CAPACITY; ++i) {
		err = da_push_within_cap(array, i);
		if (i == DA_DEFAULT_CAPACITY) {
			assert(err && "should exeed capacity");
		}
	}
	assert(da_alignto(array) == 0 && "offset of int should be 0");
	assert(da_len(array) == DA_DEFAULT_CAPACITY);
	da_pop(array);
	assert(da_len(array) == DA_DEFAULT_CAPACITY - 1);
	da_free(array);
	printf("PASSED\n");
	return 0;
}

