#ifndef INCLUDE_DARRAY_H
#define INCLUDE_DARRAY_H
#if !(defined(__GNUC__) || defined(__clang__))
#error "Compiler is not supported"
#endif

#include <stdlib.h>
#include <string.h>

#if __STDC_VERSION__ < 202311L
#define DA_TYPEOF(type) __typeof__(type)
#define DA_ALIGNOF(type) __Alignof(type)
#else
#define DA_TYPEOF(type) typeof(type)
#define DA_ALIGNOF(type) alignof(type)
#endif

#define DA_DEFAULT_CAPACITY 8

struct da_header {
	size_t len;
	size_t capacity;
};

/*
 * Returns offset needed for aligned access of array type
 */
#define da_alignto(type) ({ \
	(DA_ALIGNOF(type) - \
	    (sizeof (struct da_header) % DA_ALIGNOF(type))) % \
	    DA_ALIGNOF(type); \
})
#define da_get_header(p) ((struct da_header *)((char *)p - \
	sizeof (struct da_header) - da_alignto(*p)))
#define da_len(p) (da_get_header(p)->len)
#define da_capacity(p) (da_get_header(p)->capacity)

/*
 * Allocates space for array.
 * Receives pointer variable with type of allocated array
 * If failed, p is NULL
 */
#define da_alloc(p, c) do { \
	size_t _da_alloc_offset = da_alignto(*p); \
	(p) = (DA_TYPEOF(*(p)) *)malloc(sizeof *(p) * \
	    c + sizeof (struct da_header) + _da_alloc_offset); \
	if ((p)) { \
		((struct da_header *)(p))->len = 0; \
		((struct da_header *)(p))->capacity = c; \
		(p) = (DA_TYPEOF(*(p)) *)((char *)p + sizeof (struct da_header) + \
		    _da_alloc_offset); \
	} \
} while (0)

/*
 * Push new element to the end of array.
 * Reallocates, if capacity limit reached.
 * Returns 0 on success, otherwise error code
 */
#define da_push(p, value) ({ \
	int _da_push_err = 0; \
	if (!(p)) { \
		da_alloc(p, DA_DEFAULT_CAPACITY); \
		_da_push_err = (p == NULL); \
	} \
	if (p) { \
		if (da_len(p) == da_capacity(p)) { \
			_da_push_err = -1; \
		} \
		if (!_da_push_err) { \
			(void)(p == &value); /* typeof array and value should match */ \
			(p)[da_len(p)++] = value; \
		} \
	} \
	_da_push_err; \
})

/*
 * Pops last element from array.
 * Return 0 on success, otherwise error code
 */
#define da_pop(p) ({ \
	int _da_pop_err = 0; \
	if (da_len(p)) { \
		--da_len(p); \
	} else { \
		_da_pop_err = -1; \
	} \
	_da_pop_err; \
})

#define da_free(p) do { \
	if (p) { \
		free(da_get_header(p)); \
	} \
} while (0)

#endif
