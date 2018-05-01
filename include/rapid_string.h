#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

#ifndef __cplusplus
  #include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* TODO: other stl-like stuff? */
/* TODO: docs & comments */
/* TODO: some define for validating inputs */
/* TODO: rs_append_rs & co */
/* TODO: rs_search, rs_erase, rs_substring */
/* TODO: forward declares for all functions */
/* TODO: rs_resize_w for filler character */
/* TODO: travis (gcc and clang) & appveyor builds */
/* TODO: add coveralls */

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

#if defined(_MSC_VER) || defined (__INTEL_COMPILER)
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
#elif defined(_MSC_VER)
  #define RS_ALIGNMENT (__alignof(rs_align_dummy))
#elif __GNUC__
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
	char *buffer;
	size_t size;
	size_t capacity;
	/*
	 * Bytes will always be lost due to alignment. However, explicitly 
	 * storing the flag and the padding inside one of the union members 
	 * allows the stack string to be larger.
	 */
	uint8_t align[RS_ALIGNMENT - 1];
	/*
	 * The following flag is used to store the state of the union as well as 
	 * the remaining capacity of the stack string. They may be shared as 
	 * said cap of a stack string will always be smaller than the heap flag, 
	 * which guarentees unambiguity between the two states.
	 */
	uint8_t flag;
} rs_heap;

#define RS_MAX_SIZE (~(size_t)0)
#define RS_STACK_CAPACITY (sizeof(rs_heap) - 1)

typedef struct {
	char buffer[RS_STACK_CAPACITY];
	uint8_t left;
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

/*
 * Construction
 */
RS_API rapid_string rs_new(void);

RS_API void rs_init(rapid_string *s);

RS_API int rs_init_w(rapid_string *s, const char *input);

RS_API int rs_init_w_n(rapid_string *s, const char *input, size_t n);

RS_API int rs_init_w_cap(rapid_string *s, size_t n);

/*
 * Destruction
 */
RS_API void rs_free(rapid_string *s);

/*
 * Assignment
 */
RS_API void rs_stack_assign(rapid_string *s, const char *input);

RS_API void rs_stack_assign_n(rapid_string *s, const char *input, size_t n);

RS_API void rs_heap_assign(rapid_string *s, const char *input);

RS_API void rs_heap_assign_n(rapid_string *s, const char *input, size_t n);

RS_API int rs_assign(rapid_string *s, const char *input);

RS_API int rs_assign_n(rapid_string *s, const char *input, size_t n);

/*
 * Element access
 */
RS_API char rs_at(const rapid_string *s, size_t i);

RS_API char rs_front(const rapid_string *s);

RS_API char rs_back(const rapid_string *s);

RS_API char *rs_data(rapid_string *s);

RS_API const char *rs_data_c(const rapid_string *s);

/*
 * Iterators
 */
RS_API char *rs_begin(rapid_string *s);

RS_API const char *rs_begin_c(const rapid_string *s);

RS_API char *rs_end(rapid_string *s);

RS_API const char *rs_end_c(const rapid_string *s);

/*
 * Capacity
 */
RS_API bool rs_empty(const rapid_string *s);

RS_API size_t rs_stack_size(const rapid_string *s);

RS_API size_t rs_heap_size(const rapid_string *s);

RS_API size_t rs_size(const rapid_string *s);

RS_API size_t rs_capacity(const rapid_string *s);

RS_API int rs_reserve(rapid_string *s, size_t n);

RS_API int rs_shrink_to_fit(rapid_string *s);

RS_API bool rs_is_heap(const rapid_string *s);

RS_API bool rs_is_stack(const rapid_string *s);

/*
 * Modifiers
 */
RS_API void rs_stack_append(rapid_string *s, const char *input);

RS_API void rs_stack_append_n(rapid_string *s, const char *input, size_t n);

RS_API void rs_heap_append(rapid_string *s, const char *input);

RS_API void rs_heap_append_n(rapid_string *s, const char *input, size_t n);

RS_API int rs_append(rapid_string *s, const char *input);

RS_API int rs_append_n(rapid_string *s, const char *input, size_t n);

RS_API void rs_steal(rapid_string *s, char *buffer);

RS_API void rs_steal_n(rapid_string *s, char *buffer, size_t cap);

RS_API void rs_stack_resize(rapid_string *s, size_t n);

RS_API void rs_heap_resize(rapid_string *s, size_t n);

RS_API int rs_resize(rapid_string *s, size_t n);

/*
 * Heap operations
 */
RS_API int rs_heap_init(rapid_string *s, size_t n);

RS_API int rs_heap_init_g(rapid_string *s, size_t n);

RS_API int rs_stack_to_heap(rapid_string *s, size_t n);

RS_API int rs_stack_to_heap_g(rapid_string *s, size_t n);

RS_API int rs_realloc(rapid_string *s, size_t n);

RS_API int rs_grow_heap(rapid_string *s, size_t n);

/*
 * ===============================================================
 *
 *                          CONSTRUCTION
 *
 * ===============================================================
 */
RS_API rapid_string rs_new(void)
{
	rapid_string s;
	rs_init(&s);
	return s;
}

RS_API void rs_init(rapid_string *s)
{
	s->stack.buffer[0] = '\0';
	s->stack.left = RS_STACK_CAPACITY;
}

RS_API int rs_init_w(rapid_string *s, const char *input)
{
	return rs_init_w_n(s, input, strlen(input));
}

RS_API int rs_init_w_n(rapid_string *s, const char *input, size_t n)
{
	rs_init(s);
	return rs_assign_n(s, input, n);
}

RS_API int rs_init_w_cap(rapid_string *s, size_t n)
{
	rs_init(s);
	
	int rc = rs_heap_init(s, n);

	if (rc)
		return rc;

	rs_heap_resize(s, 0);
	return 0;
}

/*
 * ===============================================================
 *
 *                           DESTRUCTION
 *
 * ===============================================================
 */
RS_API void rs_free(rapid_string *s)
{
	if (rs_is_heap(s))
		RS_FREE(s->heap.buffer);
}

/*
 * ===============================================================
 *
 *                           ASSIGNMENT
 *
 * ===============================================================
 */
RS_API void rs_stack_assign(rapid_string *s, const char *input) 
{
	rs_stack_assign_n(s, input, strlen(input));
}

RS_API void rs_stack_assign_n(rapid_string *s, const char *input, size_t n)
{
	memcpy(s->stack.buffer, input, n);
	rs_stack_resize(s, n);
}

RS_API void rs_heap_assign(rapid_string *s, const char *input)
{
	rs_heap_assign_n(s, input, strlen(input));
}

RS_API void rs_heap_assign_n(rapid_string *s, const char *input, size_t n)
{
	memcpy(s->heap.buffer, input, n);
	rs_heap_resize(s, n);
}

RS_API int rs_assign(rapid_string *s, const char *input)
{
	return rs_assign_n(s, input, strlen(input));
}

RS_API int rs_assign_n(rapid_string *s, const char *input, size_t n) {
	if (rs_is_heap(s)) {
		const int rc = rs_grow_heap(s, n);

		if (rc)
			return rc;

		rs_heap_assign_n(s, input, n);
	} else if (n > RS_STACK_CAPACITY) {
		const int rc = rs_heap_init_g(s, n);

		if (rc)
			return rc;

		rs_heap_assign_n(s, input, n);
	} else {
		rs_stack_assign_n(s, input, n);
	}

	return 0;
}

/*
 * ===============================================================
 *
 *                         ELEMENT ACCESS
 *
 * ===============================================================
 */
RS_API char rs_at(const rapid_string *s, size_t i)
{
	return rs_data_c(s)[i];
}

RS_API char rs_front(const rapid_string *s)
{
	return *rs_begin_c(s);
}

RS_API char rs_back(const rapid_string *s)
{
	return *rs_end_c(s);
}

RS_API char *rs_data(rapid_string *s)
{
	return (char*)rs_data_c(s);
}

RS_API const char *rs_data_c(const rapid_string *s)
{
	return rs_is_heap(s) ? s->heap.buffer : s->stack.buffer;
}

/*
 * ===============================================================
 *
 *                            ITERATORS
 *
 * ===============================================================
 */
RS_API char *rs_begin(rapid_string *s)
{
	return rs_data(s);
}

RS_API const char *rs_begin_c(const rapid_string *s)
{
	return rs_data_c(s);
}

RS_API char *rs_end(rapid_string *s)
{
	return (char*)rs_end_c(s);
}

RS_API const char *rs_end_c(const rapid_string *s) 
{
	/* Manually check the flag to prevent an additional branch. */
	if (rs_is_heap(s))
		return s->heap.buffer + rs_heap_size(s);
	else
		return s->stack.buffer + rs_stack_size(s);
}

/*
 * ===============================================================
 *
 *                            CAPACITY
 *
 * ===============================================================
 */
RS_API bool rs_empty(const rapid_string *s)
{
	return rs_size(s) == 0;
}

RS_API size_t rs_stack_size(const rapid_string *s)
{
	return RS_STACK_CAPACITY - s->stack.left;
}

RS_API size_t rs_heap_size(const rapid_string *s)
{
	return s->heap.size;
}

RS_API size_t rs_size(const rapid_string *s)
{
	return rs_is_heap(s) ? 
		rs_heap_size(s) :
		rs_stack_size(s);
}

RS_API size_t rs_capacity(const rapid_string *s)
{
	return rs_is_heap(s) ? 
		s->heap.capacity :
		RS_STACK_CAPACITY;
}

RS_API int rs_reserve(rapid_string *s, size_t n) 
{
	if (rs_is_heap(s)) {
		if (s->heap.capacity >= n)
			return 0;

		return rs_realloc(s, n);
	} else {
		return rs_stack_to_heap(s, n);
	}
}

RS_API int rs_shrink_to_fit(rapid_string *s)
{
	if (rs_is_heap(s))
		return rs_realloc(s, rs_heap_size(s));

	return 0;
}

RS_API bool rs_is_heap(const rapid_string * s)
{
	return s->heap.flag == RS_HEAP_FLAG;
}

RS_API bool rs_is_stack(const rapid_string * s)
{
	return !rs_is_heap(s);
}

/*
 * ===============================================================
 *
 *                            MODIFIERS
 *
 * ===============================================================
 */
RS_API void rs_stack_append(rapid_string *s, const char *input)
{
	return rs_stack_append_n(s, input, strlen(input));
}

RS_API void rs_stack_append_n(rapid_string *s, const char *input, size_t n)
{
	const size_t stack_size = rs_stack_size(s);
	memcpy(s->stack.buffer + stack_size, input, n);
	rs_stack_resize(s, stack_size + n);
}

RS_API void rs_heap_append(rapid_string *s, const char *input)
{
	return rs_heap_append_n(s, input, strlen(input));
}

RS_API void rs_heap_append_n(rapid_string *s, const char *input, size_t n)
{
	memcpy(s->heap.buffer + rs_heap_size(s), input, n);
	rs_heap_resize(s, rs_heap_size(s) + n);
}

RS_API int rs_append(rapid_string *s, const char *input)
{
	return rs_append_n(s, input, strlen(input));
}

RS_API int rs_append_n(rapid_string *s, const char *input, size_t n)
{
	if (rs_is_heap(s)) {
		const int rc = rs_grow_heap(s, rs_heap_size(s) + n);

		if (rc)
			return rc;

		rs_heap_append_n(s, input, n);
	} else if (s->stack.left < n) {
		int rc = rs_stack_to_heap_g(s, n);

		if (rc)
			return rc;

		rs_heap_append_n(s, input, n);
	} else {
		rs_stack_append_n(s, input, n);
	}

	return 0;
}

RS_API void rs_steal(rapid_string *s, char *buffer)
{
	rs_steal_n(s, buffer, strlen(buffer));
}

RS_API void rs_steal_n(rapid_string *s, char *buffer, size_t n)
{
	/* Manual free as using rs_free creates an additional branch. */
	if (rs_is_heap(s))
		RS_FREE(s->heap.buffer);
	else
		s->heap.flag = RS_HEAP_FLAG;

	s->heap.buffer = buffer;
	s->heap.size = n;
	s->heap.capacity = n;
}

RS_API void rs_stack_resize(rapid_string *s, size_t n)
{
	s->stack.buffer[n] = '\0';
	s->stack.left = (uint8_t)(RS_STACK_CAPACITY - n);
}

RS_API void rs_heap_resize(rapid_string *s, size_t n)
{
	s->heap.buffer[n] = '\0';
	s->heap.size = n;
}

RS_API int rs_resize(rapid_string *s, size_t n)
{
	if (n > RS_STACK_CAPACITY) {
		const int rc = rs_is_heap(s) ?
			rs_reserve(s, n) :
			rs_heap_init(s, n);

		if (rc)
			return rc;

		rs_heap_resize(s, n);
	} else {
		rs_stack_resize(s, n);
	}

	return 0;
}

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */
RS_API int rs_heap_init(rapid_string *s, size_t n)
{
	s->heap.buffer = (char*)RS_MALLOC(n + 1);

	if (!s->heap.buffer)
		return RS_ERR_ALLOC;

	s->heap.capacity = n;
	s->heap.flag = RS_HEAP_FLAG;

	return 0;
}

RS_API int rs_heap_init_g(rapid_string *s, size_t n) 
{
	return rs_heap_init(s, n * RS_GROWTH_FACTOR);
}

RS_API int rs_stack_to_heap(rapid_string *s, size_t n) 
{
	const size_t stack_size = rs_stack_size(s);

	char tmp[RS_STACK_CAPACITY];
	memcpy(tmp, s->stack.buffer, stack_size);

	int rc = rs_heap_init(s, stack_size + n);

	if (rc)
		return rc;

	rs_heap_assign_n(s, tmp, stack_size);
	return 0;
}

RS_API int rs_stack_to_heap_g(rapid_string *s, size_t n) 
{
	return rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);
}

RS_API int rs_realloc(rapid_string *s, size_t n)
{
	char *new_buff = (char*)RS_REALLOC(s->heap.buffer, n + 1);

	if (!new_buff)
		return RS_ERR_ALLOC;

	s->heap.buffer = new_buff;
	s->heap.capacity = n;

	return 0;
}

RS_API int rs_grow_heap(rapid_string *s, size_t n)
{
	if (s->heap.capacity >= n)
		return 0;

	return rs_realloc(s, n * RS_GROWTH_FACTOR);
}

#ifdef __cplusplus
}
#endif

#endif /* !RAPID_STRING_H_962AB5F800398A34 */
