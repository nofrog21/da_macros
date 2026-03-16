#include "../dynamic_array.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
	int *array = NULL;
	size_t n = da_len(array);
	printf("array len: %zu\n", n);
	n = da_capacity(array);
	printf("array capacity: %zu\n", n);
	da_foreach(elem, array) {
		assert(false);
	}
	da_reserve(array, 20);
	for (int i = 0; i < (int)da_capacity(array); ++i) {
		da_push(array, i);
	}
	assert(da_capacity(array) == 20);
	da_foreach(elem, array) {
		printf("%d\n", *elem);
	}
	da_free(array);
	printf("PASSED\n");
	return 0;
}
