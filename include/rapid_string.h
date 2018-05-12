#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// TODO: other stl-like stuff?
// TODO: rs_search, rs_erase, rs_substring
// TODO: add coveralls

#define RS_HEAP_FLAG (0xFF)

#ifndef RS_GROWTH_FACTOR
  #define RS_GROWTH_FACTOR (2)
#endif

#if !defined(RS_MALLOC) && !defined(RS_REALLOC) && !defined(RS_FREE)
  #include <stdlib.h>
  #define RS_MALLOC (malloc)
  #define RS_REALLOC (realloc)
  #define RS_FREE (free)
#endif

#define RS_ASSERT(expr) do { assert(expr); } while (0)
#define RS_ASSERT_PTR(ptr) do { RS_ASSERT(ptr != NULL); } while (0)
#define RS_ASSERT_RS(s) do {					\
	RS_ASSERT_PTR(s);					\
	RS_ASSERT(s->heap.flag == RS_HEAP_FLAG ||		\
		  s->heap.flag <= RS_STACK_CAPACITY);		\
} while (0)
#define RS_ASSERT_HEAP(s) do { RS_ASSERT(rs_is_heap(s)); } while (0)
#define RS_ASSERT_STACK(s) do { RS_ASSERT(rs_is_stack(s)); } while (0)

#define RS_SIZE_DATA(f, s, input) do {					\
	if (rs_is_heap(s))						\
		f(s, input->heap.buffer, rs_heap_size(input));		\
	else								\
		f(s, input->stack.buffer, rs_stack_size(input));	\
} while (0)

#define RS_API inline

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
#elif defined(__GNUC__)
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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */

/**
 * Initializes a string.
 * @param s The string to initialize.
 */
RS_API void rs_init(rapid_string *s);

/**
 * Initializes a string with a character array.
 * Identicle to `rs_init_w_n(s, input, strlen(input);`.
 * @param s A string to initialize.
 * @param input The input used to initialize the string.
 */
RS_API void rs_init_w(rapid_string *s, const char *input);

/**
 * Initializes a string with a character array.
 * @param s A string to initialize.
 * @param input The input used to initialize the string.
 * @param n The length of the input.
 */
RS_API void rs_init_w_n(rapid_string *s, const char *input, size_t n);

/**
 * Initializes a string with an initial capacity.
 * @param s A string to initialize.
 * @param n The new initial capacity of the string.
 */
RS_API void rs_init_w_cap(rapid_string *s, size_t n);

/**
 * Initializes a string with another string.
 * @param s A string to initialize.
 * @param input The input string used to initialize `s`.
 */
RS_API void rs_init_w_rs(rapid_string *s, const rapid_string *input);

/**
 * Frees a string. The string is in an invalid state after
 * freeing. You must call `rs_init(s)` if you wish to reuse the same
 * string after freeing.
 *
 * A jump may be avoided by directly calling `RS_FREE(s->heap.buffer);` if
 * the string is known to be on the heap.
 *
 * Calling this fuction is unecessary if the string size is always smaller
 * or equal to `RS_STACK_CAPACITY`.
 * @param s The string to free.
 */
RS_API void rs_free(rapid_string *s);

/*
 * ===============================================================
 *
 *                           ASSIGNMENT
 *
 * ===============================================================
 */

/**
 * Assigns characters to a stack string. The input length must be
 * smaller or equal to `RS_STACK_CAPACITY`.
 * Identicle to `rs_stack_assign_n(s, input, strlen(input));`.
 * @param s An intialized stack string.
 * @param input the input to assign to the stack string.
 */
RS_API void rs_stack_assign(rapid_string *s, const char *input);

/**
 * Assigns characters to a stack string. The input length must be
 * smaller or equal to `RS_STACK_CAPACITY`.
 * @param s An initialized stack string.
 * @param input The input to assign to the stack string.
 * @param n The length of the input.
 */
RS_API void rs_stack_assign_n(rapid_string *s, const char *input, size_t n);

/**
 * Assigns characters to a heap string. The input length must be
 * smaller or equal to the string's capacity.
 * Identicle to `rs_heap_assign_n(s, input, strlen(input));`.
 * @param s An initialized heap string.
 * @param input The input to assign to the heap string.
 */
RS_API void rs_heap_assign(rapid_string *s, const char *input);

/**
 * Assigns characters to a heap string. The input length must be
 * smaller or equal to the string's capacity.
 * @param s An initialized heap string.
 * @param input The input to assign to the heap string.
 * @param n The length of the input.
 */
RS_API void rs_heap_assign_n(rapid_string *s, const char *input, size_t n);

/**
 * Assigns characters to a string.
 * Identicle to `rs_assign_n(s, input, strlen(input));`.
 * @param s An initialized string.
 * @param input The input to assign to the string.
 */
RS_API void rs_assign(rapid_string *s, const char *input);

/**
 * Assigns characters to a string.
 * @param s An initialized string.
 * @param input The input to assign to the string.
 * @param n The length of the input.
 */
RS_API void rs_assign_n(rapid_string *s, const char *input, size_t n);

/**
 * Assigns a rapid_string to another rapid_string.
 * @param s An initialized string.
 * @param input The input string used to initialize `s`.
 */
RS_API void rs_assign_rs(rapid_string *s, const rapid_string *input);

/*
 * ===============================================================
 *
 *                         ELEMENT ACCESS
 *
 * ===============================================================
 */

/**
 * Access a specified character.
 * @param s An initialized string.
 * @param i The index used to retrieve the character.
 * @returns The character at the index of `i`.
 */
RS_API char rs_at(const rapid_string *s, size_t i);

/**
 * Access the first character.
 * @param s An initialized string.
 * @returns The first character.
 */
RS_API char rs_front(const rapid_string *s);

/**
 * Access the last character.
 * @param s An initialized string.
 * @returns The last character.
 */
RS_API char rs_back(const rapid_string *s);

/**
 * Access the buffer.
 * @param s An initialized string.
 * returns The buffer.
 */
RS_API char *rs_data(rapid_string *s);

/**
 * Access the buffer.
 * @param s An initialized string.
 * @returns The buffer.
 */
RS_API const char *rs_data_c(const rapid_string *s);

/*
 * ===============================================================
 *
 *                            ITERATORS
 *
 * ===============================================================
 */

/**
 * Returns an iterator to the beginning.
 * @param s An intialized string.
 * @returns The iterator to the first character.
 */
RS_API char *rs_begin(rapid_string *s);

/**
 * Returns an iterator to the beginning.
 * @param s An intialized string.
 * @returns The iterator to the first character.
 */
RS_API const char *rs_begin_c(const rapid_string *s);

/**
 * Returns an iterator to the end.
 * @param s An intialized string.
 * @returns The iterator to the last character.
 */
RS_API char *rs_end(rapid_string *s);

/**
 * Returns an iterator to the end.
 * @param s An intialized string.
 * @returns The iterator to the last character.
 */
RS_API const char *rs_end_c(const rapid_string *s);

/*
 * ===============================================================
 *
 *                            CAPACITY
 *
 * ===============================================================
 */

/**
 * Checks whether a string is empty.
 * @param s An initialized string.
 * @returns `true` if the string is empty, `false` otherwise.
 */
RS_API bool rs_empty(const rapid_string *s);

/**
 * Returns the length of a stack string.
 * @param s An initialized stack string.
 * @returns The stack string length.
 */
RS_API size_t rs_stack_size(const rapid_string *s);

/**
 * Returns the length of a heap string.
 * @param s An initialized heap string.
 * @returns The heap string length.
 */
RS_API size_t rs_heap_size(const rapid_string *s);

/**
 * Returns the length.
 * @param s An initialized string.
 * @returns The string length.
 */
RS_API size_t rs_size(const rapid_string *s);

/**
 * Returns the capacity.
 * @param s An initialized string.
 * @returns The string capacity.
 */
RS_API size_t rs_capacity(const rapid_string *s);

/**
 * Reserves capacity.
 * @param s An initialized string.
 * @param n The capacity to reserve.
 */
RS_API void rs_reserve(rapid_string *s, size_t n);

/**
 * Frees all unused memory.
 * @param s An intialized string.
 */
RS_API void rs_shrink_to_fit(rapid_string *s);

/**
 * Checks whether a string is on the heap.
 * @param s An initialized string.
 * @returns `true` if the string is on the heap, `false` otherwise.
 */
RS_API bool rs_is_heap(const rapid_string *s);

/**
 * Checks whether a string is on the stack.
 * @param s An initialized string.
 * @returns `true` if the string is on the stack, `false` otherwise.
 */
RS_API bool rs_is_stack(const rapid_string *s);

/*
 * ===============================================================
 *
 *                            MODIFIERS
 *
 * ===============================================================
 */

/**
 * Appends characters to a stack string.
 * Identicle to `rs_stack_append_n(s, input, strlen(input));`.
 * @param s An initialized stack string.
 * @param input The input to append.
 */
RS_API void rs_stack_append(rapid_string *s, const char *input);

/**
 * Appends characters to a stack string.
 * @param s An initialized stack string.
 * @param input The input to append.
 * @param n The length of the input.
 */
RS_API void rs_stack_append_n(rapid_string *s, const char *input, size_t n);

/**
 * Appends characters to a heap string.
 * Identicle to `rs_heap_append_n(s, input, strlen(input));`.
 * @param s An initialized heap string.
 * @param input The input to append.
 */
RS_API void rs_heap_append(rapid_string *s, const char *input);

/**
 * Appends characters to a heap string.
 * @param s An initialized heap string.
 * @param input The input to append.
 * @param n The length of the input.
 */
RS_API void rs_heap_append_n(rapid_string *s, const char *input, size_t n);

/**
 * Appends characters to a string.
 * Identicle to `rs_append_n(s, input, strlen(input));`.
 * @param s An initialized  string.
 * @param input The input to append.
 */
RS_API void rs_append(rapid_string *s, const char *input);

/**
 * Appends characters to a string.
 * @param s An initialized  string.
 * @param input The input to append.
 * @param n The length of the input.
 */
RS_API void rs_append_n(rapid_string *s, const char *input, size_t n);

/**
 * Appends a string to another string.
 * @param s An initialized string.
 * @param input The input to append.
 */
RS_API void rs_append_rs(rapid_string *s, const rapid_string *input);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_MALLOC`/`RS_REALLOC`, or must be manually freed.
 * Identicle to `rs_steal_n(s, buffer, strlen(buffer));
 * @param s An initialized string.
 * @param buffer The buffer to steal.
 */
RS_API void rs_steal(rapid_string *s, char *buffer);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_MALLOC`/`RS_REALLOC`, or must be manually freed.
 * @param s An initialized string.
 * @param buffer The buffer to steal.
 * @param cap The capacity of the buffer.
 */
RS_API void rs_steal_n(rapid_string *s, char *buffer, size_t cap);

/**
 * Resizes a stack string. The new size must be smaller than
 * `RS_STACK_CAPACITY`.
 * @param s An initialized stack string.
 * @param n The new size.
 */
RS_API void rs_stack_resize(rapid_string *s, size_t n);

/**
 * Resizes a heap string. The new size must be smaller than `s->heap.capacity`.
 * @param s An initialized heap string.
 * @param n The new size.
 */
RS_API void rs_heap_resize(rapid_string *s, size_t n);

/**
 * Resizes a string.
 * @param s An initialized string.
 * @param n The new size.
 */
RS_API void rs_resize(rapid_string *s, size_t n);

/**
 * Resizes a string with a filler character.
 * @param s An initialized string.
 * @param n The new size.
 * @param c The filler character.
 */
RS_API void rs_resize_w(rapid_string *s, size_t n, char c);

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */

/**
 * Initializes the heap.
 * @param s A string.
 * @param n The heap capacity.
 */
RS_API void rs_heap_init(rapid_string *s, size_t n);

/**
 * Initializes the heap with growth.
 * Identicle to `rs_heap_init(s, n * RS_GROWTH_FACTOR);`.
 * @param s A string.
 * @param n The heap capacity.
 */
RS_API void rs_heap_init_g(rapid_string *s, size_t n);

/**
 * Moves a stack string to the heap.
 * @param s A initialized stack string.
 * @param n The heap capacity.
 */
RS_API void rs_stack_to_heap(rapid_string *s, size_t n);

/**
 * Moves a stack string to the heap with growth.
 * Identicle to `rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);`.
 * @param s A initialized stack string.
 * @param n The heap capacity.
 */
RS_API void rs_stack_to_heap_g(rapid_string *s, size_t n);

/**
 * Reallocates a heap buffer.
 * @param s An initialized heap string.
 * @param n The new heap capacity.
 */
RS_API void rs_realloc(rapid_string *s, size_t n);

/**
 * Allocates growth for a heap string.
 * @param s An initialized heap string.
 * @param n The new heap capacity.
 */
RS_API void rs_grow_heap(rapid_string *s, size_t n);

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */
RS_API void rs_init(rapid_string *s)
{
	RS_ASSERT_PTR(s);

	s->stack.buffer[0] = '\0';
	s->stack.left = RS_STACK_CAPACITY;
}

RS_API void rs_init_w(rapid_string *s, const char *input)
{
	rs_init_w_n(s, input, strlen(input));
}

RS_API void rs_init_w_n(rapid_string *s, const char *input, size_t n)
{
	rs_init(s);
	rs_assign_n(s, input, n);
}

RS_API void rs_init_w_cap(rapid_string *s, size_t n)
{
	rs_heap_init(s, n);
	rs_heap_resize(s, 0);
}

RS_API void rs_init_w_rs(rapid_string *s, const rapid_string *input)
{
	RS_SIZE_DATA(rs_init_w_n, s, input);
}

RS_API void rs_free(rapid_string *s)
{
	RS_ASSERT_RS(s);

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
	RS_ASSERT_PTR(input);

	rs_stack_assign_n(s, input, strlen(input));
}

RS_API void rs_stack_assign_n(rapid_string *s, const char *input, size_t n)
{
	RS_ASSERT_STACK(s);
	RS_ASSERT_PTR(input);
	RS_ASSERT(RS_STACK_CAPACITY >= n);

	memcpy(s->stack.buffer, input, n);
	rs_stack_resize(s, n);
}

RS_API void rs_heap_assign(rapid_string *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_assign_n(s, input, strlen(input));
}

RS_API void rs_heap_assign_n(rapid_string *s, const char *input, size_t n)
{
	RS_ASSERT_HEAP(s);
	RS_ASSERT_PTR(input);
	RS_ASSERT(s->heap.capacity >= n);

	memcpy(s->heap.buffer, input, n);
	rs_heap_resize(s, n);
}

RS_API void rs_assign(rapid_string *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_assign_n(s, input, strlen(input));
}

RS_API void rs_assign_n(rapid_string *s, const char *input, size_t n) {
	if (rs_is_heap(s)) {
		rs_grow_heap(s, n);
		rs_heap_assign_n(s, input, n);
	} else if (n > RS_STACK_CAPACITY) {
		rs_heap_init_g(s, n);
		rs_heap_assign_n(s, input, n);
	} else {
		rs_stack_assign_n(s, input, n);
	}
}

RS_API void rs_assign_rs(rapid_string *s, const rapid_string *input)
{
	RS_SIZE_DATA(rs_assign_n, s, input);
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
	RS_ASSERT(rs_size(s) > i);

	return rs_data_c(s)[i];
}

RS_API char rs_front(const rapid_string *s)
{
	RS_ASSERT(rs_size(s) > 0);

	return *rs_begin_c(s);
}

RS_API char rs_back(const rapid_string *s)
{
	RS_ASSERT(rs_size(s) > 0);

	return *(rs_end_c(s) - 1);
}

RS_API char *rs_data(rapid_string *s)
{
	return (char*)rs_data_c(s);
}

RS_API const char *rs_data_c(const rapid_string *s)
{
	RS_ASSERT_RS(s);

	return rs_is_heap(s) ?
		s->heap.buffer :
		s->stack.buffer;
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
	RS_ASSERT_RS(s);

	return rs_is_heap(s) ?
		s->heap.buffer + rs_heap_size(s) :
		s->stack.buffer + rs_stack_size(s);
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
	RS_ASSERT_STACK(s);

	return RS_STACK_CAPACITY - s->stack.left;
}

RS_API size_t rs_heap_size(const rapid_string *s)
{
	RS_ASSERT_HEAP(s);

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

RS_API void rs_reserve(rapid_string *s, size_t n)
{
	if (rs_is_heap(s)) {
		if (s->heap.capacity < n)
			rs_realloc(s, n);
	} else {
		rs_stack_to_heap(s, n);
	}
}

RS_API void rs_shrink_to_fit(rapid_string *s)
{
	if (rs_is_heap(s))
		rs_realloc(s, rs_heap_size(s));
}

RS_API bool rs_is_heap(const rapid_string *s)
{
	RS_ASSERT_RS(s);

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
	RS_ASSERT_PTR(input);

	rs_stack_append_n(s, input, strlen(input));
}

RS_API void rs_stack_append_n(rapid_string *s, const char *input, size_t n)
{
	RS_ASSERT_PTR(input);
	RS_ASSERT(RS_STACK_CAPACITY >= rs_stack_size(s) + n);

	const size_t stack_size = rs_stack_size(s);
	memcpy(s->stack.buffer + stack_size, input, n);
	rs_stack_resize(s, stack_size + n);
}

RS_API void rs_heap_append(rapid_string *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_append_n(s, input, strlen(input));
}

RS_API void rs_heap_append_n(rapid_string *s, const char *input, size_t n)
{
	RS_ASSERT_RS(s);
	RS_ASSERT_PTR(input);
	RS_ASSERT(s->heap.capacity >= s->heap.size + n);

	memcpy(s->heap.buffer + rs_heap_size(s), input, n);
	rs_heap_resize(s, rs_heap_size(s) + n);
}

RS_API void rs_append(rapid_string *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_append_n(s, input, strlen(input));
}

RS_API void rs_append_n(rapid_string *s, const char *input, size_t n)
{
	if (rs_is_heap(s)) {
		rs_grow_heap(s, rs_heap_size(s) + n);
		rs_heap_append_n(s, input, n);
	} else if (s->stack.left < n) {
		rs_stack_to_heap_g(s, n);
		rs_heap_append_n(s, input, n);
	} else {
		rs_stack_append_n(s, input, n);
	}
}

RS_API void rs_append_rs(rapid_string *s, const rapid_string *input)
{
	RS_SIZE_DATA(rs_append_n, s, input);
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
	RS_ASSERT(RS_STACK_CAPACITY >= n);
	
	s->stack.buffer[n] = '\0';
	s->stack.left = (uint8_t)(RS_STACK_CAPACITY - n);
}

RS_API void rs_heap_resize(rapid_string *s, size_t n)
{
	RS_ASSERT_HEAP(s);
	RS_ASSERT(s->heap.capacity >= n);

	s->heap.buffer[n] = '\0';
	s->heap.size = n;
}

RS_API void rs_resize(rapid_string *s, size_t n)
{
	if (n > RS_STACK_CAPACITY) {
		if (rs_is_heap(s))
			rs_reserve(s, n);
		else
			rs_heap_init(s, n);

		rs_heap_resize(s, n);
	} else {
		rs_stack_resize(s, n);
	}
}

RS_API void rs_resize_w(rapid_string *s, size_t n, char c)
{
	size_t sz = rs_size(s);

	rs_resize(s, n);

	if (sz < n) {
		size_t diff = n - sz;
		memset(rs_data(s) + sz, c, diff);
	}
}

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */
RS_API void rs_heap_init(rapid_string *s, size_t n)
{
	s->heap.buffer = (char*)RS_MALLOC(n + 1);
	
	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
	s->heap.flag = RS_HEAP_FLAG;
}

RS_API void rs_heap_init_g(rapid_string *s, size_t n)
{
	rs_heap_init(s, n * RS_GROWTH_FACTOR);
}

RS_API void rs_stack_to_heap(rapid_string *s, size_t n)
{
	const size_t stack_size = rs_stack_size(s);

	char tmp[RS_STACK_CAPACITY];
	memcpy(tmp, s->stack.buffer, stack_size);

	rs_heap_init(s, stack_size + n);
	rs_heap_assign_n(s, tmp, stack_size);
}

RS_API void rs_stack_to_heap_g(rapid_string *s, size_t n)
{
	rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);
}

RS_API void rs_realloc(rapid_string *s, size_t n)
{
	s->heap.buffer = (char*)RS_REALLOC(s->heap.buffer, n + 1);
	
	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
}

RS_API void rs_grow_heap(rapid_string *s, size_t n)
{
	if (s->heap.capacity < n)
		rs_realloc(s, n * RS_GROWTH_FACTOR);
}

#ifdef __cplusplus
}
#endif

#endif // !RAPID_STRING_H_962AB5F800398A34
