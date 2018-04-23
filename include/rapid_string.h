#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

#ifndef __cplusplus
	#include <stdbool.h>
#endif

#include <stddef.h>
#include <string.h>

/* TODO: other stl-like stuff? */
/* TODO: docs & comments */
/* TODO: some define for validating inputs */
/* TODO: rs_cat_rs & co */
/* TODO: rs_search, rs_erase, rs_substring */
/* TODO: forward declares for all functions */
/* TODO: rs_resize_w for filler character */
/* TODO: travis (gcc and clang) & appveyor builds */
/* TODO: add coveralls */
/* TODO: group benchmarks by test, not compiler */

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

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
	#define RS_GCC_VERSION (__GNUC__       * 10000 + \
				__GNUC_MINOR__ *   100 + \
				__GNUC_PATCHLEVEL__)
#else
	#define RS_GCC_VERSION (0)
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1500) || defined (__INTEL_COMPILER)
	#define RS_API __forceinline
#elif RS_GCC_VERSION >= 30100
	#define RS_API __attribute__((always_inline)) inline
#else
	#define RS_API inline
#endif

#ifdef __STDC_VERSION__
#define RS_C11 (__STDC_VERSION__ >= 201112L)
#else
#define RC_C11 (0)
#endif

typedef struct { void *a; size_t b; } rs_align_dummy;

#if RS_C11
	#define RS_ALIGNMENT (_Alignof(rs_align_dummy))
#elif defined(_MSC_VER) && _MSC_VER >= 1500
	#define RS_ALIGNMENT (__alignof(rs_align_dummy))
#elif RS_GCC_VERSION >= 29500
	#define RS_ALIGNMENT (__alignof__(rs_align_dummy))
#else
	/*
	 * No other way to find the alignment than assuming structs are aligned
	 * by the largest member.
	 */
	#define RS_ALIGNMENT (sizeof(void*) > sizeof(size_t) ? \
				     sizeof(void*) :           \
				     sizeof(size_t))
#endif

/*
 * Accessing packed data structures incurs a performance penalty, therefore the 
 * alignment will be used to allow for a larger stack string.
 */
typedef struct {
	char *buff;
	size_t len;
	size_t cap;
	/*
	 * Bytes will always be lost due to alignment. However, explicitly 
	 * storing the flag and the padding inside one of the union members 
	 * allows the stack string to be larger.
	 */
	unsigned char align[RS_ALIGNMENT - 1];
	/*
	 * The following flag is used to store the state of the union as well as 
	 * the remaining capacity of the stack string. They may be shared as 
	 * said cap of a stack string will always be smaller than the heap flag, 
	 * which guarentees unambiguity between the two states.
	 */
	unsigned char flag;
} rs_heap;

#define RS_STACK_CAPACITY sizeof(rs_heap) - 1

typedef struct {
	char buff[RS_STACK_CAPACITY];
	unsigned char left;
} rs_stack;

typedef union {
	rs_stack stack;
	rs_heap heap;
} rapid_string;

enum {
	RS_ERR_SUCCESS,
	RS_ERR_ALLOC
};

#ifdef __cplusplus
extern "C" {
#endif

RS_API rapid_string rs_empty(void);

RS_API void rs_init(rapid_string *str);

RS_API int rs_init_w(rapid_string *str, const char *input);

RS_API int rs_init_w_n(rapid_string *str, const char *input, size_t len);

RS_API int rs_init_w_cap(rapid_string *str, size_t len);

RS_API size_t rs_len(const rapid_string *str);

RS_API size_t rs_cap(const rapid_string *str);

RS_API bool rs_is_heap(const rapid_string *str);

RS_API bool rs_is_stack(const rapid_string *str);

RS_API char rs_at(const rapid_string *str, size_t i);

RS_API const char* rs_buff(const rapid_string *str);

RS_API int rs_cat(rapid_string *str, const char *input);

RS_API int rs_cat_n(rapid_string *str, const char *input, size_t len);

RS_API int rs_cpy(rapid_string *str, const char *input);

RS_API int rs_cpy_n(rapid_string *str, const char *input, size_t len);

RS_API void rs_steal(rapid_string *str, char *buffer);

RS_API void rs_steal_n(rapid_string *str, char *buffer, size_t cap);

RS_API int rs_reserve(rapid_string *str, size_t len);

RS_API int rs_resize(rapid_string *str, size_t len);

RS_API int rs_shrink_to_fit(rapid_string *str);

RS_API void rs_free(rapid_string *str);

RS_API size_t rs_stack_size(const rapid_string *str)
{
	/*
	 * The last element in the stack union member stores the remaining
	 * cap. Once the remaining cap reaches 0, it becomes the null
	 * terminator.
	 */
	return RS_STACK_CAPACITY - str->stack.left;
}

RS_API void rs_resize_stack(rapid_string *str, size_t len)
{
	str->stack.buff[len] = '\0';
	str->stack.left = (unsigned char)(RS_STACK_CAPACITY - len);
}

RS_API void rs_resize_heap(rapid_string *str, size_t len)
{
	str->heap.buff[len] = '\0';
	str->heap.len = len;
}

RS_API void rs_cat_heap_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->heap.buff + str->heap.len, input, len);
	rs_resize_heap(str, str->heap.len + len);
}

RS_API void rs_cat_stack_n(rapid_string *str, const char *input, size_t len)
{
	const size_t stack_size = rs_stack_size(str);
	memcpy(str->stack.buff + stack_size, input, len);
	rs_resize_stack(str, stack_size + len);
}

RS_API void rs_cpy_heap_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->heap.buff, input, len);
	rs_resize_heap(str, len);
}

RS_API void rs_cpy_stack_n(rapid_string *str, const char *input, size_t len)
{
	memcpy(str->stack.buff, input, len);
	rs_resize_stack(str, len);
}

RS_API int rs_init_heap(rapid_string *str, size_t len)
{
	str->heap.buff = (char*)RS_MALLOC(len + 1);

	if (!str->heap.buff)
		return RS_ERR_ALLOC;

	str->heap.cap = len;
	str->heap.flag = RS_HEAP_FLAG;

	return 0;
}

RS_API int rs_stack_to_heap(rapid_string *str, size_t len) 
{
	const size_t stack_size = rs_stack_size(str);

	char tmp[RS_STACK_CAPACITY];
	memcpy(tmp, str->stack.buff, stack_size);

	int rc = rs_init_heap(str, stack_size + len);

	if (rc)
		return rc;

	rs_cpy_heap_n(str, tmp, stack_size);
	return 0;
}

RS_API int rs_stack_to_heap_g(rapid_string *str, size_t len) 
{
	return rs_stack_to_heap(str, len * RS_GROWTH_FACTOR);
}

RS_API int rs_init_heap_g(rapid_string *str, size_t len) 
{
	return rs_init_heap(str, len * RS_GROWTH_FACTOR);
}

RS_API int rs_realloc(rapid_string *str, size_t len)
{
	char *new_buff = (char*)RS_REALLOC(str->heap.buff, len + 1);

	if (!new_buff)
		return RS_ERR_ALLOC;

	str->heap.buff = new_buff;
	str->heap.cap = len;

	return 0;
}

RS_API int rs_grow_heap(rapid_string *str, size_t len)
{
	if (str->heap.cap >= len)
		return 0;

	return rs_realloc(str, len * RS_GROWTH_FACTOR);
}

RS_API rapid_string rs_empty(void)
{
	rapid_string str;
	rs_init(&str);
	return str;
}

RS_API void rs_init(rapid_string *str)
{
	str->stack.buff[0] = '\0';
	str->stack.left = RS_STACK_CAPACITY;
}

RS_API int rs_init_w(rapid_string *str, const char *input)
{
	return rs_init_w_n(str, input, strlen(input));
}

RS_API int rs_init_w_n(rapid_string *str, const char *input, size_t len)
{
	rs_init(str);
	return rs_cpy_n(str, input, len);
}

RS_API int rs_init_w_cap(rapid_string *str, size_t len)
{
	rs_init(str);
	
	int rc = rs_init_heap(str, len);

	if (rc)
		return rc;

	rs_resize_heap(str, 0);
	return 0;
}

RS_API size_t rs_len(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.len :
		rs_stack_size(str);
}

RS_API size_t rs_cap(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.cap :
		RS_STACK_CAPACITY;
}

RS_API bool rs_is_heap(const rapid_string * str)
{
	return str->heap.flag == RS_HEAP_FLAG;
}

RS_API bool rs_is_stack(const rapid_string * str)
{
	return str->heap.flag != RS_HEAP_FLAG;
}

RS_API char rs_at(const rapid_string *str, size_t i)
{
	return rs_buff(str)[i];
}

RS_API const char *rs_buff(const rapid_string *str)
{
	return str->heap.flag == RS_HEAP_FLAG ? 
		str->heap.buff : 
		str->stack.buff;
}

RS_API int rs_cat(rapid_string *str, const char *input)
{
	return rs_cat_n(str, input, strlen(input));
}

RS_API int rs_cat_n(rapid_string *str, const char *input, size_t len)
{
	if (str->heap.flag == RS_HEAP_FLAG) {
		const int rc = rs_grow_heap(str, str->heap.len + len);

		if (rc)
			return rc;

		rs_cat_heap_n(str, input, len);
	} else if (str->stack.left < len) {
		int rc = rs_stack_to_heap_g(str, len);

		if (rc)
			return rc;

		rs_cat_heap_n(str, input, len);
	} else {
		rs_cat_stack_n(str, input, len);
	}

	return 0;
}

RS_API int rs_cpy(rapid_string *str, const char *input)
{
	return rs_cpy_n(str, input, strlen(input));
}

RS_API int rs_cpy_n(rapid_string *str, const char *input, size_t len) {
	if (str->heap.flag == RS_HEAP_FLAG) {
		const int rc = rs_grow_heap(str, len);

		if (rc)
			return rc;

		rs_cpy_heap_n(str, input, len);
	} else if (len > RS_STACK_CAPACITY) {
		const int rc = rs_init_heap_g(str, len);

		if (rc)
			return rc;

		rs_cpy_heap_n(str, input, len);
	} else {
		rs_cpy_stack_n(str, input, len);
	}

	return 0;
}

RS_API void rs_steal(rapid_string *str, char *buffer)
{
	rs_steal_n(str, buffer, strlen(buffer));
}

RS_API void rs_steal_n(rapid_string *str, char *buffer, size_t len)
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

RS_API int rs_reserve(rapid_string *str, size_t len) 
{
	if (str->heap.flag == RS_HEAP_FLAG) {
		if (str->heap.cap >= len)
			return 0;

		return rs_realloc(str, len);
	} else {
		return rs_stack_to_heap(str, len);
	}
}

RS_API int rs_resize(rapid_string *str, size_t len)
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

RS_API int rs_shrink_to_fit(rapid_string *str)
{
	if (str->heap.flag == RS_HEAP_FLAG)
		return rs_realloc(str, str->heap.len);

	return 0;
}

RS_API void rs_free(rapid_string *str)
{
	if (str->heap.flag == RS_HEAP_FLAG)
		RS_FREE(str->heap.buff);
}

#ifdef __cplusplus
}
#endif

#endif /* !RAPID_STRING_H_962AB5F800398A34 */
