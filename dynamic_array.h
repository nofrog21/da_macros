#ifndef INCLUDE_DARRAY_H
#define INCLUDE_DARRAY_H
#if !(defined(__GNUC__) || defined(__clang__))
#error "Compiler is not supported"
#endif

#include <stdlib.h>
#include <string.h>

#if __STDC_VERSION__ < 202311L
#define DA_TYPEOF(type) __typeof__(type)
#define DA_ALIGNOF(type) __alignof__(type)
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

/*
 * Expects begining of the array
 * If passed NULL, will return NULL
 */
#define da_unsafe_get_header(p) (p ? (struct da_header *)((char *)p - \
	sizeof (struct da_header) - da_alignto(*p)) : NULL)
/*
 * Return len and capacity
 */
#define da_len(p) (p ? da_unsafe_get_header(p)->len : 0)
#define da_capacity(p) (p ? da_unsafe_get_header(p)->capacity : 0)

/*
 * Allocates space for array.
 * Receives pointer variable with type of allocated array and capacity
 * If failed, p is NULL
 */
#define da_alloc(p, c) do { \
	(p) = (DA_TYPEOF(p)) aligned_alloc(DA_ALIGNOF(*(p)), sizeof (*(p)) * \
	    c + sizeof (struct da_header) + da_alignto(*(p))); \
	if ((p)) { \
		((struct da_header *)(p))->len = 0; \
		((struct da_header *)(p))->capacity = c; \
		(p) = (DA_TYPEOF(*(p)) *)((char *)p + sizeof (struct da_header) + \
		    da_alignto(*(p))); \
	} \
} while (0)

/*
 * Reserve specified capacity
 * On failier fill return error, array will not be modified
 * if capacity is less than previous will not do anything
 * All pointers to previous memory will become invalid and
 * de-referencing them is use after free
 */
#define da_reserve(p, c) ({ \
	da_err _da_rsv_err = 0; \
	if (!p) { \
		da_alloc(p, c); \
	} else if (c > da_capacity(p)) { \
		DA_TYPEOF(p) _da_new_p = NULL; \
		da_alloc(_da_new_p, c); \
		if (_da_new_p) { \
			memcpy(_da_new_p, p, sizeof *(p) * da_len(p)); \
			da_free(p); \
			p = _da_new_p; \
		} else { \
			_da_rsv_err = DA_ERR_OOM; \
		} \
	} \
	_da_rsv_err; \
})

/*
 * Gets pointer to the last element.
 */
#define da_peek(p) ({ \
	(p && da_len(p) && \
	(da_len(p) <= da_capacity(p)) ? p + da_len(p) - 1 : NULL); \
})


/*
 * Push new element to end of the array.
 * Reallocates if capacity limit is reached.
 * Returns 0 on success, error code on failier.
 * Will not invalidate data if failed.
 */
#define da_push(p, value) ({ \
	da_err _da_push_err = da_push_within_cap(p, value); \
	if (_da_push_err) { \
		_da_push_err = da_reserve(p, 2 * da_capacity(p)); \
		if (!_da_push_err) { \
			_da_push_err = da_push_within_cap(p, value); \
		} \
	} \
	_da_push_err; \
})

/*
 * Push new element to the end of the array.
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
			(p)[da_unsafe_get_header(p)->len++] = value; \
		} \
	} \
	_da_push_err; \
})

/*
 * Pops last element from the array.
 * Return 0 on success, otherwise error code
 */
#define da_pop(p) ({ \
	da_err _da_pop_err = 0; \
	if (p) { \
		if (da_len(p)) { \
			--da_unsafe_get_header(p)->len; \
		} else { \
			_da_pop_err = DA_ERR_EMPTY; \
		} \
	} else { \
		_da_pop_err = DA_FAIL; \
	} \
	_da_pop_err; \
})

/*
 * Frees the array.
 * Should not be called on memory not owned by array.
 * Safe to call on NULL pointer.
 */
#define da_free(p) do { \
	if (p) { \
		free(da_unsafe_get_header(p)); \
	} \
} while (0)


/*
 * `elem` is a pointer
 */
#define da_foreach(elem, p) \
	for (DA_TYPEOF(p) elem = p; \
		elem; elem = (elem == da_peek(p) ? NULL : elem + 1))

#endif
