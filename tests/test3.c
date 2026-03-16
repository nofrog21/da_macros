#include "../dynamic_array.h"

#include <stdio.h>

int main(void)
{
	int *array = NULL;
	size_t n = da_len(array);
	printf("array len: %zu\n", n);
	n = da_capacity(array);
	printf("array capacity: %zu\n", n);
	printf("PASSED\n");
	return 0;
}
