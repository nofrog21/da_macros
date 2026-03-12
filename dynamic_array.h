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

typedef enum {
	DA_OK,
	DA_FAIL,
	DA_ERR_OOM,
	DA_ERR_CAP_LIMIT,
	DA_ERR_EMPTY,
} da_err;

/*
 * Returns offset needed for aligned access of array type
 */
#define da_alignto(type) ({ \
	(DA_ALIGNOF(type) - \
	    (sizeof (struct da_header) % DA_ALIGNOF(type))) % \
	    DA_ALIGNOF(type); \
})

#define da_get_header(p) (p ? (struct da_header *)((char *)p - \
	sizeof (struct da_header) - da_alignto(*p)) : NULL)
#define da_len(p) (da_get_header(p)->len)
#define da_capacity(p) (da_get_header(p)->capacity)

/*
 * Allocates space for array.
 * Receives pointer variable with type of allocated array and capacity
 * If failed, p is NULL
 */
#define da_alloc(p, c) do { \
	(p) = (DA_TYPEOF(*(p)) *)malloc(sizeof (*(p)) * \
	    c + sizeof (struct da_header) + da_alignto(*(p))); \
	if ((p)) { \
		((struct da_header *)(p))->len = 0; \
		((struct da_header *)(p))->capacity = c; \
		(p) = (DA_TYPEOF(*(p)) *)((char *)p + sizeof (struct da_header) + \
		    da_alignto(*(p))); \
	} \
} while (0)

/*
 * Gets pointer to last element.
 * Returns NULL is len is past capacity.
 */
#define da_peek(p) ({ \
	(p && (da_len(p) <= da_capacity(p)) ? p + da_len(p) - 1 : NULL); \
})


/*
 * Push new element to end of array.
 * Reallocates if capacity limit is reached.
 * Returns 0 on success, error code on failier.
 * Will not invalidate data if failed.
 */
#define da_push(p, value) ({ \
	da_err _da_push_err = da_push_within_cap(p, value); \
	if (_da_push_err) { \
		DA_TYPEOF(p) _da_push_p; \
		da_alloc(_da_push_p, 2 * da_capacity(p)); \
		if (_da_push_p) { \
			memcpy(_da_push_p, p, sizeof(*p) * da_len(p)); \
			da_free(p); \
			p = _da_push_p; \
		} else { \
			_da_push_err = DA_ERR_OOM;\
		} \
	} \
	_da_push_err; \
})

/*
 * Push new element to the end of array.
 * Will not reallocate if capacity limit is reached.
 * Returns 0 on success, otherwise error code
 */
#define da_push_within_cap(p, value) ({ \
	da_err _da_push_err = 0; \
	if (!(p)) { \
		da_alloc(p, DA_DEFAULT_CAPACITY); \
		_da_push_err = (p == NULL ? DA_ERR_OOM : 0); \
	} \
	if (p) { \
		if (da_len(p) >= da_capacity(p)) { \
			_da_push_err = DA_ERR_CAP_LIMIT; \
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
	da_err _da_pop_err = 0; \
	if (p) { \
		if (da_len(p)) { \
			--da_len(p); \
		} else { \
			_da_pop_err = DA_ERR_EMPTY; \
		} \
	} else { \
		_da_pop_err = DA_FAIL; \
	} \
	_da_pop_err; \
})

/*
 * Frees array.
 * Should not be called on memory not owned by array.
 * Safe to call on NULL pointer.
 */
#define da_free(p) do { \
	if (p) { \
		free(da_get_header(p)); \
	} \
} while (0)

#endif
