/*
 * rapidstring - A fast string library.
 * version 1.0.0
 * https://github.com/boyerjohn/rapidstring
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2018 John Boyer <john.boyer@tutanota.com>.
 */

/*
 *       TABLE OF CONTENTS
 *
 * 1. CONSTRUCTION & DESTRUCTION
 * - Declarations:	line 212
 * - Defintions:	line 745
 *
 * 2. ASSIGNMENT
 * - Declarations:	line 270
 * - Defintions:	line 791
 *
 * 3. ELEMENT ACCESS
 * - Declarations:	line 337
 * - Defintions:	line 857
 *
 * 4. ITERATORS
 * - Declarations:	line 381
 * - Defintions:	line 900
 *
 * 5. CAPACITY
 * - Declarations:	line 417
 * - Defintions:	line 932
 *
 * 6. MODIFIERS
 * - Declarations:	line 487
 * - Defintions:	line 1001
 *
 * 7. HEAP OPERATIONS
 * - Declarations:	line 598
 * - Defintions:	line 1129
 *
 * 8. STACK ALLOCATOR
 * - Declarations:	line 652
 * - Defintions:	line 1184
 *
 * 9. DEFAULT ALLOCATOR
 * - Declarations:	line 707
 * - Defintions:	line 1252
 */

#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

#define RS_VERSION_MAJOR 0
#define RS_VERSION_MINOR 1
#define RS_VERSION_PATCH 0

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// TODO: organize macros & structs in toc
// TODO: rename to c like methods
// TODO: other stl-like stuff?
// TODO: rs_search, rs_erase, rs_substring
// TODO: add coveralls

#ifndef RS_GROWTH_FACTOR
  #define RS_GROWTH_FACTOR (2)
#endif

#ifndef RS_AVERAGE_SIZE
  #define RS_AVERAGE_SIZE (50)
#endif

#ifndef RSA_STACK_SIZE
  #define RSA_STACK_SIZE 16384
#endif

#if !defined(RS_ALLOC) && !defined(RS_REALLOC) && !defined(RS_FREE)
  #define RS_ALLOC (rsa_alloc)
  #define RS_REALLOC (rsa_realloc)
  #define RS_FREE (rsa_free)
#endif

#define RS_HEAP_FLAG (0xFF)

#define RS_ASSERT_PTR(ptr) do { assert(ptr != NULL); } while (0)
#define RS_ASSERT_RS(s) do {					\
	RS_ASSERT_PTR(s);					\
	assert(s->heap.flag == RS_HEAP_FLAG ||		\
		  s->heap.flag <= RS_STACK_CAPACITY);		\
} while (0)
#define RS_ASSERT_HEAP(s) do { assert(rs_is_heap(s)); } while (0)
#define RS_ASSERT_STACK(s) do { assert(rs_is_stack(s)); } while (0)

#ifdef __GNUC__
  #define RS_GCC_VERSION (__GNUC__ * 10000 +		\
			  __GNUC_MINOR__ * 100 +	\
			  __GNUC_PATCHLEVEL__)
#else
  #define RS_GCC_VERSION (0)
#endif

#if RS_GCC_VERSION > 29600
  #define RS_EXPECT(expr, val) __builtin_expect((expr), val)
#else
  #define RS_EXPECT(expr, val) (expr)
#endif

#define RS_LIKELY(expr) RS_EXPECT(expr, 1)
#define RS_UNLIKELY(expr) RS_EXPECT(expr, 0)

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
  #define RS_ALIGNMENT (sizeof(void*) > sizeof(size_t) ?	\
			       sizeof(void*) :			\
			       sizeof(size_t))
#endif

/**
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

#define RS_STACK_CAPACITY (sizeof(rs_heap) - 1)

typedef struct {
	char buffer[RS_STACK_CAPACITY];
	uint8_t left;
} rs_stack;

typedef union {
	rs_stack stack;
	rs_heap heap;
} rapidstring;

typedef struct {
	uint8_t buff[RSA_STACK_SIZE];
	uint8_t* ptr;
} rsa_stack_t;

static rsa_stack_t rsa_stack = { {}, rsa_stack.buff };

// Based off the average string size, allow for more efficient branching.
enum { RS_HEAP_LIKELY_V = RS_AVERAGE_SIZE > RS_STACK_CAPACITY };

#define RS_HEAP_LIKELY(expr) RS_EXPECT(expr, RS_HEAP_LIKELY_V)
#define RS_STACK_LIKELY(expr) RS_EXPECT(expr, !RS_HEAP_LIKELY_V)

/**
 * Forwards the buffer and size of a string to the provided function.
 * Retrieving both the buffer and the size of a string requires a flag check,
 * which would result in an additional branch if not done manually.
 * @param f A function.
 * @param s An initialized string.
 * @param input The input to forward the function.
 */
#define RS_DATA_SIZE(f, s, input) do {					\
	if (RS_HEAP_LIKELY(rs_is_heap(input)))				\
		f(s, input->heap.buffer, rs_heap_size(input));		\
	else								\
		f(s, input->stack.buffer, rs_stack_size(input));	\
} while (0)

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
static inline void rs_init(rapidstring *s);

/**
 * Initializes a string with a character array.
 * Identicle to `rs_init_w_n(s, input, strlen(input);`.
 * @param s A string to initialize.
 * @param input The input used to initialize the string.
 */
static inline void rs_init_w(rapidstring *s, const char *input);

/**
 * Initializes a string with a character array.
 * @param s A string to initialize.
 * @param input The input used to initialize the string.
 * @param n The length of the input.
 */
static inline void rs_init_w_n(rapidstring *s, const char *input, size_t n);

/**
 * Initializes a string with an initial capacity.
 * @param s A string to initialize.
 * @param n The new initial capacity of the string.
 */
static inline void rs_init_w_cap(rapidstring *s, size_t n);

/**
 * Initializes a string with another string.
 * @param s A string to initialize.
 * @param input The input string used to initialize `s`.
 */
static inline void rs_init_w_rs(rapidstring *s, const rapidstring *input);

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
static inline void rs_free(rapidstring *s);

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
static inline void rs_stack_assign(rapidstring *s, const char *input);

/**
 * Assigns characters to a stack string. The input length must be
 * smaller or equal to `RS_STACK_CAPACITY`.
 * @param s An initialized stack string.
 * @param input The input to assign to the stack string.
 * @param n The length of the input.
 */
static inline void rs_stack_assign_n(rapidstring *s, const char *input, size_t n);

/**
 * Assigns characters to a heap string. The input length must be
 * smaller or equal to the string's capacity.
 * Identicle to `rs_heap_assign_n(s, input, strlen(input));`.
 * @param s An initialized heap string.
 * @param input The input to assign to the heap string.
 */
static inline void rs_heap_assign(rapidstring *s, const char *input);

/**
 * Assigns characters to a heap string. The input length must be
 * smaller or equal to the string's capacity.
 * @param s An initialized heap string.
 * @param input The input to assign to the heap string.
 * @param n The length of the input.
 */
static inline void rs_heap_assign_n(rapidstring *s, const char *input, size_t n);

/**
 * Assigns characters to a string.
 * Identicle to `rs_assign_n(s, input, strlen(input));`.
 * @param s An initialized string.
 * @param input The input to assign to the string.
 */
static inline void rs_assign(rapidstring *s, const char *input);

/**
 * Assigns characters to a string.
 * @param s An initialized string.
 * @param input The input to assign to the string.
 * @param n The length of the input.
 */
static inline void rs_assign_n(rapidstring *s, const char *input, size_t n);

/**
 * Assigns a string to another string.
 * @param s An initialized string.
 * @param input The input string used to initialize `s`.
 */
static inline void rs_assign_rs(rapidstring *s, const rapidstring *input);

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
static inline char rs_at(const rapidstring *s, size_t i);

/**
 * Access the first character.
 * @param s An initialized string.
 * @returns The first character.
 */
static inline char rs_front(const rapidstring *s);

/**
 * Access the last character.
 * @param s An initialized string.
 * @returns The last character.
 */
static inline char rs_back(const rapidstring *s);

/**
 * Access the buffer.
 * @param s An initialized string.
 * @returns The buffer.
 */
static inline char *rs_data(rapidstring *s);

/**
 * Access the buffer.
 * @param s An initialized string.
 * @returns The buffer.
 */
static inline const char *rs_data_c(const rapidstring *s);

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
static inline char *rs_begin(rapidstring *s);

/**
 * Returns an iterator to the beginning.
 * @param s An intialized string.
 * @returns The iterator to the first character.
 */
static inline const char *rs_begin_c(const rapidstring *s);

/**
 * Returns an iterator to the end.
 * @param s An intialized string.
 * @returns The iterator to the last character.
 */
static inline char *rs_end(rapidstring *s);

/**
 * Returns an iterator to the end.
 * @param s An intialized string.
 * @returns The iterator to the last character.
 */
static inline const char *rs_end_c(const rapidstring *s);

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
static inline bool rs_empty(const rapidstring *s);

/**
 * Returns the length of a stack string.
 * @param s An initialized stack string.
 * @returns The stack string length.
 */
static inline size_t rs_stack_size(const rapidstring *s);

/**
 * Returns the length of a heap string.
 * @param s An initialized heap string.
 * @returns The heap string length.
 */
static inline size_t rs_heap_size(const rapidstring *s);

/**
 * Returns the length.
 * @param s An initialized string.
 * @returns The string length.
 */
static inline size_t rs_size(const rapidstring *s);

/**
 * Returns the capacity.
 * @param s An initialized string.
 * @returns The string capacity.
 */
static inline size_t rs_capacity(const rapidstring *s);

/**
 * Reserves capacity.
 * @param s An initialized string.
 * @param n The capacity to reserve.
 */
static inline void rs_reserve(rapidstring *s, size_t n);

/**
 * Frees all unused memory.
 * @param s An intialized string.
 */
static inline void rs_shrink_to_fit(rapidstring *s);

/**
 * Checks whether a string is on the heap.
 * @param s An initialized string.
 * @returns `true` if the string is on the heap, `false` otherwise.
 */
static inline bool rs_is_heap(const rapidstring *s);

/**
 * Checks whether a string is on the stack.
 * @param s An initialized string.
 * @returns `true` if the string is on the stack, `false` otherwise.
 */
static inline bool rs_is_stack(const rapidstring *s);

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
static inline void rs_stack_append(rapidstring *s, const char *input);

/**
 * Appends characters to a stack string.
 * @param s An initialized stack string.
 * @param input The input to append.
 * @param n The length of the input.
 */
static inline void rs_stack_append_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends characters to a heap string.
 * Identicle to `rs_heap_append_n(s, input, strlen(input));`.
 * @param s An initialized heap string.
 * @param input The input to append.
 */
static inline void rs_heap_append(rapidstring *s, const char *input);

/**
 * Appends characters to a heap string.
 * @param s An initialized heap string.
 * @param input The input to append.
 * @param n The length of the input.
 */
static inline void rs_heap_append_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends characters to a string.
 * Identicle to `rs_append_n(s, input, strlen(input));`.
 * @param s An initialized  string.
 * @param input The input to append.
 */
static inline void rs_append(rapidstring *s, const char *input);

/**
 * Appends characters to a string.
 * @param s An initialized  string.
 * @param input The input to append.
 * @param n The length of the input.
 */
static inline void rs_append_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends a string to another string.
 * @param s An initialized string.
 * @param input The input to append.
 */
static inline void rs_append_rs(rapidstring *s, const rapidstring *input);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_ALLOC`/`RS_REALLOC`, or must be manually freed.
 * Identicle to `rs_steal_n(s, buffer, strlen(buffer));
 * @param s An initialized string.
 * @param buffer The buffer to steal.
 */
static inline void rs_steal(rapidstring *s, char *buffer);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_ALLOC`/`RS_REALLOC`, or must be manually freed.
 * @param s An initialized string.
 * @param buffer The buffer to steal.
 * @param cap The capacity of the buffer.
 */
static inline void rs_steal_n(rapidstring *s, char *buffer, size_t cap);

/**
 * Resizes a stack string. The new size must be smaller than
 * `RS_STACK_CAPACITY`.
 * @param s An initialized stack string.
 * @param n The new size.
 */
static inline void rs_stack_resize(rapidstring *s, size_t n);

/**
 * Resizes a heap string. The new size must be smaller than `s->heap.capacity`.
 * @param s An initialized heap string.
 * @param n The new size.
 */
static inline void rs_heap_resize(rapidstring *s, size_t n);

/**
 * Resizes a string.
 * @param s An initialized string.
 * @param n The new size.
 */
static inline void rs_resize(rapidstring *s, size_t n);

/**
 * Resizes a string with a filler character.
 * @param s An initialized string.
 * @param n The new size.
 * @param c The filler character.
 */
static inline void rs_resize_w(rapidstring *s, size_t n, char c);

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */

/**
 * Initializes the heap. Intended for internal use.
 * @param s A string.
 * @param n The heap capacity.
 */
static inline void rs_heap_init(rapidstring *s, size_t n);

/**
 * Initializes the heap with growth. Intended for internal use.
 * Identicle to `rs_heap_init(s, n * RS_GROWTH_FACTOR);`.
 * @param s A string.
 * @param n The heap capacity.
 */
static inline void rs_heap_init_g(rapidstring *s, size_t n);

/**
 * Moves a stack string to the heap. Intended for internal use.
 * @param s A initialized stack string.
 * @param n The heap capacity.
 */
static inline void rs_stack_to_heap(rapidstring *s, size_t n);

/**
 * Moves a stack string to the heap with growth. Intended for internal use.
 * Identicle to `rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);`.
 * @param s A initialized stack string.
 * @param n The heap capacity.
 */
static inline void rs_stack_to_heap_g(rapidstring *s, size_t n);

/**
 * Reallocates the heap buffer. This method may grow or shrink the heap
 * capacity. The size will remain the same, even if the new capacity is smaller
 * than the current size. Intended for internal use.
 * @param s An initialized heap string.
 * @param n The new heap capacity.
 */
static inline void rs_realloc(rapidstring *s, size_t n);

/**
 * Allocates growth for a heap string. Intended for internal use.
 * @param s An initialized heap string.
 * @param n The new heap capacity.
 */
static inline void rs_grow_heap(rapidstring *s, size_t n);

/*
 * ===============================================================
 *
 *                         STACK ALLOCATOR
 *
 * ===============================================================
 */

/**
 * Checks whether `n` bytes can be allocated.
 * @param n Number of bytes.
 * @return `true` if `n` bytes can be allocated, `false` otherwise.
 */
static inline bool rsa_stack_can_alloc(size_t n);

/**
 * Allocates bytes of zero initialized storage on the stack. If the size is
 * zero, a valid pointer with no usable storage is returned.
 * @param n Number of bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_stack_free()`. On failure, returns a
 * null pointer.
 */
static inline void *rsa_stack_alloc(size_t n);

/**
 * Reallocates the given area of memory. It must have been previously allocated
 * with `rsa_stack_alloc()` or `rsa_stack_realloc()` and not yet freed with
 * `rsa_stack_free()`. If the size is zero, `p` will not be freed and a
 * valid pointer with no usable storage will be returned.
 * @param p Pointer to an area of memory to reallocate.
 * @param sz Current size of the area of memory in bytes.
 * @param n New size of the area of memory in bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_stack_free()`. On failure, returns a
 * null pointer.
 */
static inline void *rsa_stack_realloc(void *p, size_t sz, size_t n);

/**
 * Checks whether `p` is owned by this stack allocator.
 * @param p Pointer to an area of memory.
 * @return `true` if `p` is owned by this stack allocator, `false` otherwise.
 */
static inline bool rsa_stack_owns(void *p);

/**
 * Deallocates the space previously allocated by `rsa_stack_alloc()` or
 * `rsa_stack_realloc()`. If the pointer is null, the function does nothing.
 * The behavior is undefined if the same pointer is freed more than once.
 * @param p Pointer to the memory to deallocate.
 * @param n Size of the memory.
 */
static inline void rsa_stack_free(void *p, size_t n);

/*
 * ===============================================================
 *
 *                        DEFAULT ALLOCATOR
 *
 * ===============================================================
 */

/**
 * Allocates bytes of uninitialized storage.
 * @param n Number of bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rs_free`. On failure, returns a null
 * pointer.
 */
static inline void *rsa_alloc(size_t n);

/**
 * Reallocates the given area of memory. It must have been previously allocated
 * with `rsa_alloc()` or `rsa_realloc()` and not yet freed with `rsa_free()`.
 * @param p Pointer to an area of memory to reallocate.
 * @param sz Current size of the area of memory in bytes.
 * @param n New size of the area of memory in bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_free()`. On failure, returns a null
 * pointer.
 */
static inline void *rsa_realloc(void *p, size_t sz, size_t n);

/**
 * Deallocates the space previously allocated by `rsa_alloc()` or
 * `rsa_realloc()`. If the pointer is null, the function does nothing.
 * The behavior is undefined if the same pointer is freed more than once.
 * @param p Pointer to the memory to deallocate.
 * @param n Size of the memory.
 */
static inline void rsa_free(void *p, size_t n);

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */

static inline void rs_init(rapidstring *s)
{
	RS_ASSERT_PTR(s);

	s->stack.buffer[0] = '\0';
	s->stack.left = RS_STACK_CAPACITY;
}

static inline void rs_init_w(rapidstring *s, const char *input)
{
	rs_init_w_n(s, input, strlen(input));
}

static inline void rs_init_w_n(rapidstring *s, const char *input, size_t n)
{
	rs_init(s);
	rs_assign_n(s, input, n);
}

static inline void rs_init_w_cap(rapidstring *s, size_t n)
{
	rs_heap_init(s, n);
	rs_heap_resize(s, 0);
}

static inline void rs_init_w_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_init_w_n, s, input);
}

static inline void rs_free(rapidstring *s)
{
	RS_ASSERT_RS(s);

	if (RS_HEAP_LIKELY(rs_is_heap(s)))
		RS_FREE(s->heap.buffer, s->heap.capacity + 1);
}

/*
 * ===============================================================
 *
 *                           ASSIGNMENT
 *
 * ===============================================================
 */

static inline void rs_stack_assign(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_stack_assign_n(s, input, strlen(input));
}

static inline void rs_stack_assign_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_STACK(s);
	RS_ASSERT_PTR(input);
	assert(RS_STACK_CAPACITY >= n);

	memcpy(s->stack.buffer, input, n);
	rs_stack_resize(s, n);
}

static inline void rs_heap_assign(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_assign_n(s, input, strlen(input));
}

static inline void rs_heap_assign_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_HEAP(s);
	RS_ASSERT_PTR(input);
	assert(s->heap.capacity >= n);

	memcpy(s->heap.buffer, input, n);
	rs_heap_resize(s, n);
}

static inline void rs_assign(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_assign_n(s, input, strlen(input));
}

static inline void rs_assign_n(rapidstring *s, const char *input, size_t n) {
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		rs_grow_heap(s, n);
		rs_heap_assign_n(s, input, n);
	} else if (RS_HEAP_LIKELY(n > RS_STACK_CAPACITY)) {
		rs_heap_init_g(s, n);
		rs_heap_assign_n(s, input, n);
	} else {
		rs_stack_assign_n(s, input, n);
	}
}

static inline void rs_assign_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_assign_n, s, input);
}

/*
 * ===============================================================
 *
 *                         ELEMENT ACCESS
 *
 * ===============================================================
 */

static inline char rs_at(const rapidstring *s, size_t i)
{
	assert(rs_size(s) > i);

	return rs_data_c(s)[i];
}

static inline char rs_front(const rapidstring *s)
{
	assert(rs_size(s) > 0);

	return *rs_begin_c(s);
}

static inline char rs_back(const rapidstring *s)
{
	assert(rs_size(s) > 0);

	return *(rs_end_c(s) - 1);
}

static inline char *rs_data(rapidstring *s)
{
	return (char*)rs_data_c(s);
}

static inline const char *rs_data_c(const rapidstring *s)
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

static inline char *rs_begin(rapidstring *s)
{
	return rs_data(s);
}

static inline const char *rs_begin_c(const rapidstring *s)
{
	return rs_data_c(s);
}

static inline char *rs_end(rapidstring *s)
{
	return (char*)rs_end_c(s);
}

static inline const char *rs_end_c(const rapidstring *s)
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

static inline bool rs_empty(const rapidstring *s)
{
	return rs_size(s) == 0;
}

static inline size_t rs_stack_size(const rapidstring *s)
{
	RS_ASSERT_STACK(s);

	return RS_STACK_CAPACITY - s->stack.left;
}

static inline size_t rs_heap_size(const rapidstring *s)
{
	RS_ASSERT_HEAP(s);

	return s->heap.size;
}

static inline size_t rs_size(const rapidstring *s)
{
	return rs_is_heap(s) ?
		rs_heap_size(s) :
		rs_stack_size(s);
}

static inline size_t rs_capacity(const rapidstring *s)
{
	return rs_is_heap(s) ?
		s->heap.capacity :
		RS_STACK_CAPACITY;
}

static inline void rs_reserve(rapidstring *s, size_t n)
{
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		if (RS_LIKELY(s->heap.capacity < n))
			rs_realloc(s, n);
	} else {
		rs_stack_to_heap(s, n);
	}
}

static inline void rs_shrink_to_fit(rapidstring *s)
{
	if (RS_LIKELY(rs_is_heap(s)))
		rs_realloc(s, rs_heap_size(s));
}

static inline bool rs_is_heap(const rapidstring *s)
{
	RS_ASSERT_RS(s);

	return s->heap.flag == RS_HEAP_FLAG;
}

static inline bool rs_is_stack(const rapidstring *s)
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

static inline void rs_stack_append(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_stack_append_n(s, input, strlen(input));
}

static inline void rs_stack_append_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_PTR(input);
	assert(RS_STACK_CAPACITY >= rs_stack_size(s) + n);

	const size_t stack_size = rs_stack_size(s);
	memcpy(s->stack.buffer + stack_size, input, n);
	rs_stack_resize(s, stack_size + n);
}

static inline void rs_heap_append(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_append_n(s, input, strlen(input));
}

static inline void rs_heap_append_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_RS(s);
	RS_ASSERT_PTR(input);
	assert(s->heap.capacity >= s->heap.size + n);

	memcpy(s->heap.buffer + rs_heap_size(s), input, n);
	rs_heap_resize(s, rs_heap_size(s) + n);
}

static inline void rs_append(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_append_n(s, input, strlen(input));
}

static inline void rs_append_n(rapidstring *s, const char *input, size_t n)
{
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		rs_grow_heap(s, rs_heap_size(s) + n);
		rs_heap_append_n(s, input, n);
	} else if (RS_HEAP_LIKELY(s->stack.left < n)) {
		rs_stack_to_heap_g(s, n);
		rs_heap_append_n(s, input, n);
	} else {
		rs_stack_append_n(s, input, n);
	}
}

static inline void rs_append_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_append_n, s, input);
}

static inline void rs_steal(rapidstring *s, char *buffer)
{
	rs_steal_n(s, buffer, strlen(buffer));
}

static inline void rs_steal_n(rapidstring *s, char *buffer, size_t n)
{
	/* Manual free as using rs_free creates an additional branch. */
	if (RS_HEAP_LIKELY(rs_is_heap(s)))
		RS_FREE(s->heap.buffer, s->heap.capacity + 1);
	else
		s->heap.flag = RS_HEAP_FLAG;

	s->heap.buffer = buffer;
	s->heap.size = n;
	s->heap.capacity = n;
}

static inline void rs_stack_resize(rapidstring *s, size_t n)
{
	assert(RS_STACK_CAPACITY >= n);
	
	s->stack.buffer[n] = '\0';
	s->stack.left = (uint8_t)(RS_STACK_CAPACITY - n);
}

static inline void rs_heap_resize(rapidstring *s, size_t n)
{
	RS_ASSERT_HEAP(s);
	assert(s->heap.capacity >= n);

	s->heap.buffer[n] = '\0';
	s->heap.size = n;
}

static inline void rs_resize(rapidstring *s, size_t n)
{
	if (RS_HEAP_LIKELY(n > RS_STACK_CAPACITY)) {
		if (RS_HEAP_LIKELY(rs_is_heap(s)))
			rs_reserve(s, n);
		else
			rs_heap_init(s, n);

		rs_heap_resize(s, n);
	} else {
		rs_stack_resize(s, n);
	}
}

static inline void rs_resize_w(rapidstring *s, size_t n, char c)
{
	size_t sz = rs_size(s);

	rs_resize(s, n);

	if (RS_LIKELY(sz < n)) {
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

static inline void rs_heap_init(rapidstring *s, size_t n)
{
	s->heap.buffer = (char*)RS_ALLOC(n + 1);

	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
	s->heap.flag = RS_HEAP_FLAG;
}

static inline void rs_heap_init_g(rapidstring *s, size_t n)
{
	rs_heap_init(s, n * RS_GROWTH_FACTOR);
}

static inline void rs_stack_to_heap(rapidstring *s, size_t n)
{
	const size_t stack_size = rs_stack_size(s);

	char tmp[RS_STACK_CAPACITY];
	memcpy(tmp, s->stack.buffer, stack_size);

	rs_heap_init(s, stack_size + n);
	rs_heap_assign_n(s, tmp, stack_size);
}

static inline void rs_stack_to_heap_g(rapidstring *s, size_t n)
{
	rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);
}

static inline void rs_realloc(rapidstring *s, size_t n)
{
	s->heap.buffer = (char*)RS_REALLOC(s->heap.buffer,
					   s->heap.capacity + 1, n + 1);

	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
}

static inline void rs_grow_heap(rapidstring *s, size_t n)
{
	if (RS_UNLIKELY(s->heap.capacity < n))
		rs_realloc(s, n * RS_GROWTH_FACTOR);
}

/*
 * ===============================================================
 *
 *                         STACK ALLOCATOR
 *
 * ===============================================================
 */

static inline bool rsa_stack_can_alloc(size_t n)
{
	const uint8_t *end = rsa_stack.buff + RSA_STACK_SIZE;
	return n <= (size_t)(end - rsa_stack.ptr);
}

static inline void *rsa_stack_alloc(size_t n)
{
	if (RS_UNLIKELY(!rsa_stack_can_alloc(n)))
		return NULL;

	void *tmp = rsa_stack.ptr;
	rsa_stack.ptr += n;
	return tmp;
}

static inline void *rsa_stack_realloc(void *p, size_t sz, size_t n)
{
	if (RS_UNLIKELY(!p))
		return rsa_stack_alloc(n);

	assert(rsa_stack_owns(p));

	uint8_t *end = rsa_stack.ptr - sz;
	bool shrink = sz > n;
	bool is_end = (uint8_t*)p == end;

	if (RS_LIKELY(is_end && (shrink || rsa_stack_can_alloc(n - sz)))) {
		rsa_stack.ptr = end + n;
		return p;
	} else {
		void *buff = rsa_stack_alloc(n);

		if (!buff)
			return NULL;

		memcpy(buff, p, sz);
		return buff;
	}
}

static inline bool rsa_stack_owns(void *p)
{
	RS_ASSERT_PTR(p);

	uint8_t *ptr = (uint8_t*)p;

	return ptr >= rsa_stack.buff &&
	       ptr < rsa_stack.buff + RSA_STACK_SIZE;
}

static inline void rsa_stack_free(void *p, size_t n)
{
	if (RS_UNLIKELY(!p))
		return;

	uint8_t *ptr = (uint8_t*)p;

	if (RS_LIKELY(n + ptr == rsa_stack.ptr))
		rsa_stack.ptr = ptr;
}

/*
 * ===============================================================
 *
 *                        DEFAULT ALLOCATOR
 *
 * ===============================================================
 */

static inline void *rsa_alloc(size_t n)
{
	void *p = rsa_stack_alloc(n);

	if (!p)
		return malloc(n);

	return p;
}

static inline void *rsa_realloc(void *p, size_t sz, size_t n)
{
	if (rsa_stack_owns(p)) {
		if (rsa_stack_realloc(p, sz, n))
			return p;

		void *buff = malloc(n);

		if (!buff)
			return NULL;

		memcpy(buff, p, sz);
		rsa_stack_free(p, sz);
		return buff;
	} else {
		return realloc(p, n);
	}
}

static inline void rsa_free(void *p, size_t n)
{
	if (rsa_stack_owns(p)) {
		rsa_stack_free(p, n);
	} else {
		free(p);
	}
}

#endif // !RAPID_STRING_H_962AB5F800398A34
