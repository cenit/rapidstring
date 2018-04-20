#include "rapid_string.h"
#include <string.h>

#define RS_HEAP_FLAG (0xFF)

#ifndef RS_GROWTH_FACTOR
	#define RS_GROWTH_FACTOR (2)
#endif

/* If all three allocation methods aren't defined, redefine them. */
#if !defined(RS_MALLOC) || !defined(RS_REALLOC) || !defined(RS_FREE)
	#ifdef RS_MALLOC
		#undef RS_MALLOC
	#endif
	#ifdef RS_REALLOC
		#undef RS_REALLOC
	#endif
	#ifdef RS_FREE
		#undef RS_FREE
	#endif

	/* Define the allocation methods. */
	#include <stdlib.h>
	#define RS_MALLOC (malloc)
	#define RS_REALLOC (realloc)
	#define RS_FREE (free)
#endif

#if RS_GCC_VERSION >= 30100 && RS_C99
	#define RS_INLINE_DEF RS_INLINE inline
#elif defined(__INTEL_COMPILER)
	#define RS_INLINE_DEF
#else
	#define RS_INLINE_DEF RS_INLINE
#endif

#if RS_GCC_VERSION >= 40700 && !RS_C99
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wattributes"
#endif

static RS_INLINE_DEF size_t
rs_stack_size(const rapid_string *str)
{
	/*
	 * The last element in the stack union member stores the remaining
	 * cap. Once the remaining cap reaches 0, it becomes the null
	 * terminator.
	 */
	return RS_STACK_CAPACITY - str->stack.left;
}

static RS_INLINE_DEF void
rs_resize_stack(rapid_string *str, size_t len)
{
	str->stack.buff[len] = '\0';
	str->stack.left = (unsigned char)(RS_STACK_CAPACITY - len);
}

static RS_INLINE_DEF void
rs_resize_heap(rapid_string *str, size_t len)
{
	str->heap.buff[len] = '\0';
	str->heap.len = len;
}

static RS_INLINE_DEF void
rs_cat_heap_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->heap.buff + str->heap.len, input, len);
	rs_resize_heap(str, str->heap.len + len);
}

static RS_INLINE_DEF void
rs_cat_stack_n(rapid_string *str, const char *input, size_t len)
{
	const size_t stack_size = rs_stack_size(str);
	memcpy(str->stack.buff + stack_size, input, len);
	rs_resize_stack(str, stack_size + len);
}

static RS_INLINE_DEF void
rs_cpy_heap_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->heap.buff, input, len);
	rs_resize_heap(str, len);
}

static RS_INLINE_DEF void
rs_cpy_stack_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->stack.buff, input, len);
	rs_resize_stack(str, len);
}

static RS_INLINE_DEF int
rs_init_heap(rapid_string *str, size_t len)
{
	str->heap.buff = RS_MALLOC((len * RS_GROWTH_FACTOR) + 1);

	if (!str->heap.buff)
		return RS_ERR_ALLOC;

	str->heap.cap = len;
	str->heap.flag = RS_HEAP_FLAG;

	return 0;
}

static RS_INLINE_DEF int
rs_realloc(rapid_string *str, size_t len)
{
	char *new_buff = RS_REALLOC(str->heap.buff, len + 1);

	if (!new_buff)
		return RS_ERR_ALLOC;

	str->heap.buff = new_buff;
	str->heap.cap = len;

	return 0;
}

static RS_INLINE_DEF int
rs_reserve_growth(rapid_string *str, size_t len)
{
	if (str->heap.cap >= len)
		return 0;

	return rs_realloc(str, len * RS_GROWTH_FACTOR);
}

rapid_string rs_empty(void)
{
	rapid_string str;
	rs_init(&str);
	return str;
}

void rs_init(rapid_string *str)
{
	str->stack.buff[0] = '\0';
	str->stack.left = RS_STACK_CAPACITY;
}

int rs_init_w(rapid_string *str, const char *input)
{
	return rs_init_w_n(str, input, strlen(input));
}

int rs_init_w_n(rapid_string *str, const char *input, size_t len)
{
	rs_init(str);
	return rs_cpy_n(str, input, len);
}

size_t rs_len(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.len :
		rs_stack_size(str);
}

size_t rs_cap(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.cap :
		RS_STACK_CAPACITY;
}

int rs_is_heap(const rapid_string * str)
{
	return str->heap.flag == RS_HEAP_FLAG;
}

int rs_is_stack(const rapid_string * str)
{
	return str->heap.flag != RS_HEAP_FLAG;
}

char rs_at(const rapid_string *str, size_t i)
{
	return rs_buff(str)[i];
}

const char *rs_buff(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.buff : 
		str->stack.buff;
}

int rs_cat(rapid_string *str, const char *input)
{
	return rs_cat_n(str, input, strlen(input));
}

int rs_cat_n(rapid_string *str, const char *input, size_t len)
{
	if (str->heap.flag == RS_HEAP_FLAG) {
		const int rc = rs_reserve_growth(str, str->heap.len + len);

		if (rc)
			return rc;

		rs_cat_heap_n(str, input, len);
	} else if (str->stack.left < len) {
		int rc;
		const size_t stack_size = rs_stack_size(str);

		/* Create a temporary buffer for the stack string. */
		char tmp[RS_STACK_CAPACITY];
		memcpy(tmp, str->stack.buff, stack_size);

		rc = rs_init_heap(str, stack_size + len);

		if (rc)
			return rc;

		/* Copy the stack buffer to the new heap buffer. */
		rs_cpy_heap_n(str, tmp, stack_size);
		rs_cat_heap_n(str, input, len);
	} else {
		rs_cat_stack_n(str, input, len);
	}

	return 0;
}

int rs_cpy(rapid_string *str, const char *input)
{
	return rs_cpy_n(str, input, strlen(input));
}

int rs_cpy_n(rapid_string *str, const char *input, size_t len) {
	if (str->heap.flag == RS_HEAP_FLAG) {
		const int rc = rs_reserve_growth(str, len);

		if (rc)
			return rc;

		rs_cpy_heap_n(str, input, len);
	} else if (len > RS_STACK_CAPACITY) {
		const int rc = rs_init_heap(str, len);

		if (rc)
			return rc;

		rs_cpy_heap_n(str, input, len);
	} else {
		rs_cpy_stack_n(str, input, len);
	}

	return 0;
}

void rs_steal(rapid_string *str, char *buffer)
{
	rs_steal_n(str, buffer, strlen(buffer));
}

void rs_steal_n(rapid_string *str, char *buffer, size_t len)
{
	/* Manual free as using rs_free creates an additional branch. */
	if (str->heap.flag == RS_HEAP_FLAG)
		RS_FREE(str->heap.buff);
	else
		str->heap.flag = RS_HEAP_FLAG;

	str->heap.buff = buffer;
	str->heap.len = len;
	str->heap.cap = len;
}

int rs_reserve(rapid_string *str, size_t len) {
	if (str->heap.cap >= len)
		return 0;

	return rs_realloc(str, len);
}

int rs_resize(rapid_string *str, size_t len)
{
	if (len > RS_STACK_CAPACITY) {
		const int rc = str->heap.flag == RS_HEAP_FLAG ?
			rs_reserve(str, len) :
			rs_init_heap(str, len);

		if (rc)
			return rc;

		rs_resize_heap(str, len);
	} else {
		rs_resize_stack(str, len);
	}

	return 0;
}

int rs_shrink_to_fit(rapid_string *str)
{
	if (str->heap.flag == RS_HEAP_FLAG)
		return rs_realloc(str, str->heap.len);

	return 0;
}

void rs_free(rapid_string *str)
{
	if (str->heap.flag == RS_HEAP_FLAG)
		RS_FREE(str->heap.buff);
}

#if RS_GCC_VERSION >= 40700 && !RS_C99
	#pragma GCC diagnostic pop
#endif
