/*
 * rapidstring - A fast string library.
 * version 0.1.0
 * https://github.com/boyerjohn/rapidstring
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2018 John Boyer <john.boyer@tutanota.com>.
 */

/*
 *       TABLE OF CONTENTS
 *
 * 1. STRUCTURES & MACROS
 * - Declarations:	line 95
 *
 * 2. CONSTRUCTION & DESTRUCTION
 * - Declarations:	line 334
 * - Defintions:	line 816
 *
 * 3. ASSIGNMENT
 * - Declarations:	line 395
 * - Defintions:	line 862
 *
 * 4. ELEMENT ACCESS
 * - Declarations:	line 463
 * - Defintions:	line 928
 *
 * 5. CAPACITY
 * - Declarations:	line 485
 * - Defintions:	line 950
 *
 * 6. MODIFIERS
 * - Declarations:	line 555
 * - Defintions:	line 1019
 *
 * 7. HEAP OPERATIONS
 * - Declarations:	line 666
 * - Defintions:	line 1149
 *
 * 8. STACK ALLOCATOR
 * - Declarations:	line 720
 * - Defintions:	line 1204
 *
 * 9. DEFAULT ALLOCATOR
 * - Declarations:	line 775
 * - Defintions:	line 1282
 */

/**
 * @file rapidstring.h
 * @brief The single header of the rapidstring library.
 */

/**
 * @mainpage rapidstring
 * The documentation of the rapidstring library.
 */

#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

/**
 * @todo Add complexity, briefs and versions to all function documentation.
 *
 * @todo Make "Intended for internal use." a warning in Doxygen.
 *
 * @todo Make the stack allocator thread safe with RSA_THREADSAFE defined.
 *
 * @todo Make all functions properly link in documentation.
 *
 * @todo Rename to c like methods, rs_cat instead of rs_cat, etc.
 *
 * @todo Make sure all std::string methods are added (if applicable).
 *
 * @todo Create rs_search, rs_erase, rs_substring.
 *
 * @todo int return values with errno for malloc failure.
 *
 * @todo Add coveralls.
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * ===============================================================
 *
 *                       STRUCTURES & MACROS
 *
 * ===============================================================
 */

#define RS_VERSION_MAJOR 0
#define RS_VERSION_MINOR 1
#define RS_VERSION_PATCH 0

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
	assert(s->heap.flag == RS_HEAP_FLAG ||			\
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
  /* GCC version 2.96 required for branch prediction expectation. */
  #define RS_EXPECT(expr, val) __builtin_expect((expr), val)
#else
  #define RS_EXPECT(expr, val) (expr)
#endif

#define RS_LIKELY(expr) RS_EXPECT(expr, 1)
#define RS_UNLIKELY(expr) RS_EXPECT(expr, 0)

#ifdef __STDC_VERSION__
  #define RS_C99 (__STDC_VERSION__ >= 199901L)
  #define RS_C11 (__STDC_VERSION__ >= 201112L)
#else
  #define RS_C99 (0)
  #define RC_C11 (0)
#endif

#ifdef NDEBUG
  #define RS_API static
#else
  /* GCC version 3.1 required for the always inline attribute. */
  #if RS_GCC_VERION > 30100
    #define RS_API static __inline__ __attribute__((always_inline))
  #elif defined(__GNUC__)
    #define RS_API static __inline__
  #elif defined(_MSC_VER)
    #define RS_API static __forceinline
  #elif RS_C99
    #define RS_API static inline
  #else
    #define RS_API static
  #endif
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
 * @brief Struct that stores the heap data.
 *
 * A struct that stores the heap data and a flag with explicit alignment.
 * Accessing packed data structures incurs a performance penalty, therefore the
 * alignment will be used to allow for a larger stack string.
 */
typedef struct {
	/**
	 * @brief Buffer of a heap string.
	 *
	 * The buffer of a heap string allocated using `RS_ALLOC` or
	 * `RS_REALLOC`. This buffer may be manually freed by directly calling
	 * `RS_FREE(s->buffer, s->capacity + 1)`. Doing so will avoid a flag
	 * check. The additional one is for the null terminator, which is
	 * subtracted upon initial allocation.
	 */
	char *buffer;
	/**
	 * @brief Size of a heap string.
	 *
	 * The number of characters in a heap string. The null terminator is
	 * not included.
	 */
	size_t size;
	/**
	 * @brief Capacity of a heap string.
	 *
	 * The capacity of a heap string. The null terminator is not included.
	 */
	size_t capacity;
	/**
	 * @brief Alignnment of a heap string.
	 *
	 * An explicit alignment to ensure `flag` and `left` are stored in the
	 * same location.
	 */
	unsigned char align[RS_ALIGNMENT - 1];
	/**
	 * @brief Flag of the rapidstring union.
	 *
	 * The following flag is used to store the state of the union as well
	 * as the remaining capacity of the stack string. They may be shared as
	 * the capacity of a stack string will always be smaller than
	 * `RS_HEAP_FLAG`, which guarentees unambiguity between the two states.
	 */
	unsigned char flag;
} rs_heap;

/**
 * @brief Capacity of a stack string.
 *
 * The maximum capacity of a stack string.
 */
#define RS_STACK_CAPACITY (sizeof(rs_heap) - 1)

/**
 * @brief Struct that stores the stack data.
 *
 * A struct that stores the stack buffer and capacity left.
 */
typedef struct {
	/**
	 * @brief Buffer of a stack string.
	 *
	 * An array of characters the size of #RS_STACK_CAPACITY exlcuding the
	 * null terminator.
	 */
	char buffer[RS_STACK_CAPACITY];
	/**
	 * @brief The capacity left in the buffer of a stack string.
	 *
	 * The capacity left in the buffer of stack string. Once the stack
	 * string runs out of space, zero will be written to this member,
	 * effectively becoming the null terminator.
	 */
	unsigned char left;
} rs_stack;

/**
 * @brief Union that stores a rapidstring.
 *
 * A union that stores a rapidstring which is either on the heap or the stack.
 * All API methods are prefixed with `rs_heap_x()` or `rs_stack_x()`. These
 * methods are to be used only when a string is guarenteed to be in either
 * state, as mismatching these methods results in undefined behavior. There
 * will always be the `rs_x()` alternative which automatically handles the
 * state of the string. These methods should be used if optimization isn't a
 * necessity or if strings have highly variable sizes.
 */
typedef union {
	/**
	 * @brief Stack state of the rapidstring union.
	 *
	 * The stack state of the rapidstring union.
	 */
	rs_stack stack;
	/**
	 * @brief Heap state of the rapidstring union.
	 *
	 * The heap state of the rapidstring union.
	 */
	rs_heap heap;
} rapidstring;

typedef struct {
	unsigned char buff[RSA_STACK_SIZE];
	unsigned char* ptr;
} rsa_stack_t;

static rsa_stack_t rsa_stack = { { 0 }, rsa_stack.buff };

/* Based off the average string size, allow for more efficient branching. */
enum { RS_HEAP_LIKELY_V = RS_AVERAGE_SIZE > RS_STACK_CAPACITY };

#define RS_HEAP_LIKELY(expr) RS_EXPECT(expr, RS_HEAP_LIKELY_V)
#define RS_STACK_LIKELY(expr) RS_EXPECT(expr, !RS_HEAP_LIKELY_V)

/**
 * @brief Pass the string data and size to a function.
 *
 * Forwards the buffer and size of a string to the provided function.
 * Retrieving both the buffer and the size of a string requires a flag check,
 * which would result in an additional branch if not done manually. Intended
 * for internal use.
 *
 * @param[in] f A function.
 * @param[in,out] s An initialized string.
 * @param[in] input The input to forward the function.
 */
#define RS_DATA_SIZE(f, s, input) do {					\
	if (RS_HEAP_LIKELY(rs_is_heap(input)))				\
		f(s, input->heap.buffer, rs_heap_len(input));		\
	else								\
		f(s, input->stack.buffer, rs_stack_len(input));		\
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
 * @param[out] s The string to initialize.
 */
RS_API void rs_init(rapidstring *s);

/**
 * Initializes a string with a character array.
 * Identicle to `rs_init_w_n(s, input, strlen(input)`.
 * @param[out] s A string to initialize.
 * @param[in] input The input used to initialize the string.
 */
RS_API void rs_init_w(rapidstring *s, const char *input);

/**
 * Initializes a string with a character array.
 * @param[out] s A string to initialize.
 * @param[in] input The input used to initialize the string.
 * @param[in] n The length of the input.
 */
RS_API void rs_init_w_n(rapidstring *s, const char *input, size_t n);

/**
 * Initializes a string with an initial capacity.
 * @param[out] s A string to initialize.
 * @param[in] n The new initial capacity of the string.
 */
RS_API void rs_init_w_cap(rapidstring *s, size_t n);

/**
 * Initializes a string with another string.
 * @param[out] s A string to initialize.
 * @param[in] input The input string used to initialize `s`.
 */
RS_API void rs_init_w_rs(rapidstring *s, const rapidstring *input);

/**
 * Frees a string. The string is in an invalid state after
 * freeing. You must call `rs_init(s)` if you wish to reuse the same
 * string after freeing.
 *
 * A jump may be avoided by directly calling
 * `RS_FREE(s->heap.buffer, s->heap.capacity + 1);` if the string is known to
 * be on the heap. The additional one is for the null terminator, which is
 * subtracted upon initial allocation.
 *
 * Calling this fuction is unecessary if the string size is always smaller
 * or equal to #RS_STACK_CAPACITY.
 *
 * @param[in] s The string to free.
 */
RS_API void rs_free(rapidstring *s);

/*
 * ===============================================================
 *
 *                           ASSIGNMENT
 *
 * ===============================================================
 */

/**
 * Copies characters to a stack string, overwriting any existing data. The
 * input length must be smaller or equal to #RS_STACK_CAPACITY.
 * Identicle to `rs_stack_cpy_n(s, input, strlen(input))`.
 * @param[in,out] s An intialized stack string.
 * @param[in] input the input to assign to the stack string.
 */
RS_API void rs_stack_cpy(rapidstring *s, const char *input);

/**
 * Copies characters to a stack string, overwriting any existing data. The
 * input length must be smaller or equal to #RS_STACK_CAPACITY.
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to assign to the stack string.
 * @param[in] n The length of the input.
 */
RS_API void rs_stack_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * Copies characters to a heap string, overwriting any existing data. The input
 * length must be smaller or equal to the string's capacity. 
 * Identicle to `rs_heap_cpy_n(s, input, strlen(input))`.
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to assign to the heap string.
 */
RS_API void rs_heap_cpy(rapidstring *s, const char *input);

/**
 * Copies characters to a heap string, overwriting any existing data. The input
 * length must be smaller or equal to the string's capacity.
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to assign to the heap string.
 * @param[in] n The length of the input.
 */
RS_API void rs_heap_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * Copies characters to a string, overwriting any existing data.
 * Identicle to `rs_cpy_n(s, input, strlen(input))`.
 * @param[in,out] s An initialized string.
 * @param[in] input The input to assign to the string.
 */
RS_API void rs_cpy(rapidstring *s, const char *input);

/**
 * Copies characters to a string, overwriting any existing data.
 * @param[in,out] s An initialized string.
 * @param[in] input The input to assign to the string.
 * @param[in] n The length of the input.
 */
RS_API void rs_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * Copies characters from a string to another string, overwriting any existing
 * data.
 * @param[in, out] s An initialized string.
 * @param[in] input The input string used to initialize `s`.
 */
RS_API void rs_cpy_rs(rapidstring *s, const rapidstring *input);

/*
 * ===============================================================
 *
 *                         ELEMENT ACCESS
 *
 * ===============================================================
 */

/**
 * Access the buffer.
 * @param[in] s An initialized string.
 * @returns The buffer.
 */
RS_API char *rs_data(rapidstring *s);

/**
 * Access the buffer.
 * @param[in] s An initialized string.
 * @returns The buffer.
 */
RS_API const char *rs_data_c(const rapidstring *s);

/*
 * ===============================================================
 *
 *                            CAPACITY
 *
 * ===============================================================
 */

/**
 * Checks whether a string is empty.
 * @param[in] s An initialized string.
 * @returns `1` if the string is empty, `0` otherwise.
 */
RS_API int rs_empty(const rapidstring *s);

/**
 * Returns the length of a stack string.
 * @param[in] s An initialized stack string.
 * @returns The stack string length.
 */
RS_API size_t rs_stack_len(const rapidstring *s);

/**
 * Returns the length of a heap string.
 * @param[in] s An initialized heap string.
 * @returns The heap string length.
 */
RS_API size_t rs_heap_len(const rapidstring *s);

/**
 * Returns the length.
 * @param[in] s An initialized string.
 * @returns The string length.
 */
RS_API size_t rs_len(const rapidstring *s);

/**
 * Returns the capacity.
 * @param[in] s An initialized string.
 * @returns The string capacity.
 */
RS_API size_t rs_capacity(const rapidstring *s);

/**
 * Reserves capacity.
 * @param[in,out] s An initialized string.
 * @param[in] n The capacity to reserve.
 */
RS_API void rs_reserve(rapidstring *s, size_t n);

/**
 * Frees all unused memory.
 * @param[in,out] s An intialized string.
 */
RS_API void rs_shrink_to_fit(rapidstring *s);

/**
 * Checks whether a string is on the heap.
 * @param[in] s An initialized string.
 * @returns `1` if the string is on the heap, `0` otherwise.
 */
RS_API int rs_is_heap(const rapidstring *s);

/**
 * Checks whether a string is on the stack.
 * @param[in] s An initialized string.
 * @returns `1` if the string is on the stack, `0` otherwise.
 */
RS_API int rs_is_stack(const rapidstring *s);

/*
 * ===============================================================
 *
 *                            MODIFIERS
 *
 * ===============================================================
 */

/**
 * Appends characters to a stack string.
 * Identicle to `rs_stack_cat_n(s, input, strlen(input))`.
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to append.
 */
RS_API void rs_stack_cat(rapidstring *s, const char *input);

/**
 * Appends characters to a stack string.
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to append.
 * @param[in] n The length of the input.
 */
RS_API void rs_stack_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends characters to a heap string.
 * Identicle to `rs_heap_cat_n(s, input, strlen(input))`.
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to append.
 */
RS_API void rs_heap_cat(rapidstring *s, const char *input);

/**
 * Appends characters to a heap string.
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to append.
 * @param[in] n The length of the input.
 */
RS_API void rs_heap_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends characters to a string.
 * denticle to `rs_cat_n(s, input, strlen(input))`.
 * @param[in,out] s An initialized  string.
 * @param[in] input The input to append.
 */
RS_API void rs_cat(rapidstring *s, const char *input);

/**
 * Appends characters to a string.
 * @param[in,out] s An initialized  string.
 * @param[in] input The input to append.
 * @param[in] n The length of the input.
 */
RS_API void rs_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * Appends a string to another string.
 * @param[in,out] s An initialized string.
 * @param[in] input The input to append.
 */
RS_API void rs_cat_rs(rapidstring *s, const rapidstring *input);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_ALLOC`/`RS_REALLOC`, or must be manually freed.
 * Identicle to `rs_steal_n(s, buffer, strlen(buffer))
 * @param[in,out] s An initialized string.
 * @param[in] buffer The buffer to steal.
 */
RS_API void rs_steal(rapidstring *s, char *buffer);

/**
 * Steals a buffer allocated on the heap. The buffer must either be allocated
 * with `RS_ALLOC`/`RS_REALLOC`, or must be manually freed.
 * @param[in,out] s An initialized string.
 * @param[in] buffer The buffer to steal.
 * @param[in] cap The capacity of the buffer.
 */
RS_API void rs_steal_n(rapidstring *s, char *buffer, size_t cap);

/**
 * Resizes a stack string. The new size must be smaller than
 * #RS_STACK_CAPACITY.
 * @param[in,out] s An initialized stack string.
 * @param[in] n The new size.
 */
RS_API void rs_stack_resize(rapidstring *s, size_t n);

/**
 * Resizes a heap string. The new size must be smaller than `s->heap.capacity`.
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new size.
 */
RS_API void rs_heap_resize(rapidstring *s, size_t n);

/**
 * Resizes a string.
 * @param[in,out] s An initialized string.
 * @param[in] n The new size.
 */
RS_API void rs_resize(rapidstring *s, size_t n);

/**
 * Resizes a string with a filler character.
 * @param[in,out] s An initialized string.
 * @param[in] n The new size.
 * @param[in] c The filler character.
 */
RS_API void rs_resize_w(rapidstring *s, size_t n, char c);

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */

/**
 * Initializes the heap. Intended for internal use.
 * @param[out] s A string to initialize.
 * @param[in] n The heap capacity.
 */
RS_API void rs_heap_init(rapidstring *s, size_t n);

/**
 * Initializes the heap with growth. Intended for internal use.
 * Identicle to `rs_heap_init(s, n * RS_GROWTH_FACTOR)`.
 * @param[out] s A string to initialize.
 * @param[in] n The heap capacity.
 */
RS_API void rs_heap_init_g(rapidstring *s, size_t n);

/**
 * Moves a stack string to the heap. Intended for internal use.
 * @param[in,out] s An initialized stack string.
 * @param[in] n The heap capacity.
 */
RS_API void rs_stack_to_heap(rapidstring *s, size_t n);

/**
 * Moves a stack string to the heap with growth. Intended for internal use.
 * Identicle to `rs_stack_to_heap(s, n * RS_GROWTH_FACTOR)`.
 * @param[in,out] s An initialized stack string.
 * @param[in] n The heap capacity.
 */
RS_API void rs_stack_to_heap_g(rapidstring *s, size_t n);

/**
 * Reallocates the heap buffer. This method may grow or shrink the heap
 * capacity. The size will remain the same, even if the new capacity is smaller
 * than the current size. Intended for internal use.
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new heap capacity.
 */
RS_API void rs_realloc(rapidstring *s, size_t n);

/**
 * Allocates growth for a heap string. Intended for internal use.
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new heap capacity.
 */
RS_API void rs_grow_heap(rapidstring *s, size_t n);

/*
 * ===============================================================
 *
 *                         STACK ALLOCATOR
 *
 * ===============================================================
 */

/**
 * Checks whether `n` bytes can be allocated.
 * @param[in] n Number of bytes.
 * @return `1` if `n` bytes can be allocated, `0` otherwise.
 */
RS_API int rsa_stack_can_alloc(size_t n);

/**
 * Allocates bytes of zero initialized storage on the stack. If the size is
 * zero, a valid pointer with no usable storage is returned.
 * @param[in] n Number of bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_stack_free()`. On failure, returns a
 * null pointer.
 */
RS_API void *rsa_stack_alloc(size_t n);

/**
 * Reallocates the given area of memory. It must have been previously allocated
 * with `rsa_stack_alloc()` or `rsa_stack_realloc()` and not yet freed with
 * `rsa_stack_free()`. If the size is zero, `p` will not be freed and a
 * valid pointer with no usable storage will be returned.
 * @param[in] p Pointer to an area of memory to reallocate.
 * @param[in] sz Current size of the area of memory in bytes.
 * @param[in] n New size of the area of memory in bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_stack_free()`. On failure, returns a
 * null pointer.
 */
RS_API void *rsa_stack_realloc(void *p, size_t sz, size_t n);

/**
 * Checks whether `p` is owned by this stack allocator.
 * @param[in] p Pointer to an area of memory.
 * @return `1` if `p` is owned by this stack allocator, `0` otherwise.
 */
RS_API int rsa_stack_owns(void *p);

/**
 * Deallocates the space previously allocated by `rsa_stack_alloc()` or
 * `rsa_stack_realloc()`. If the pointer is null, the function does nothing.
 * The behavior is undefined if the same pointer is freed more than once.
 * @param[in] p Pointer to the memory to deallocate.
 * @param[in] n Size of the memory.
 */
RS_API void rsa_stack_free(void *p, size_t n);

/*
 * ===============================================================
 *
 *                        DEFAULT ALLOCATOR
 *
 * ===============================================================
 */

/**
 * Allocates bytes of uninitialized storage.
 *
 * @param[in] n Number of bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rs_free`. On failure, returns a null
 * pointer.
 */
RS_API void *rsa_alloc(size_t n);

/**
 * Reallocates the given area of memory. It must have been previously allocated
 * with `rsa_alloc()` or `rsa_realloc()` and not yet freed with `rsa_free()`.
 *
 * @param[in] p Pointer to an area of memory to reallocate.
 * @param[in] sz Current size of the area of memory in bytes.
 * @param[in] n New size of the area of memory in bytes.
 * @return On success, returns a pointer to the first byte of allocated memory.
 * This pointer must be passed to `rsa_free()`. On failure, returns a null
 * pointer.
 */
RS_API void *rsa_realloc(void *p, size_t sz, size_t n);

/**
 * Deallocates the space previously allocated by `rsa_alloc()` or
 * `rsa_realloc()`. If the pointer is null, the function does nothing.
 * The behavior is undefined if the same pointer is freed more than once.
 *
 * @param[in] p Pointer to the memory to deallocate.
 * @param[in] n Size of the memory.
 */
RS_API void rsa_free(void *p, size_t n);

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */

RS_API void rs_init(rapidstring *s)
{
	RS_ASSERT_PTR(s);

	s->stack.buffer[0] = '\0';
	s->stack.left = RS_STACK_CAPACITY;
}

RS_API void rs_init_w(rapidstring *s, const char *input)
{
	rs_init_w_n(s, input, strlen(input));
}

RS_API void rs_init_w_n(rapidstring *s, const char *input, size_t n)
{
	rs_init(s);
	rs_cpy_n(s, input, n);
}

RS_API void rs_init_w_cap(rapidstring *s, size_t n)
{
	rs_heap_init(s, n);
	rs_heap_resize(s, 0);
}

RS_API void rs_init_w_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_init_w_n, s, input);
}

RS_API void rs_free(rapidstring *s)
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

RS_API void rs_stack_cpy(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_stack_cpy_n(s, input, strlen(input));
}

RS_API void rs_stack_cpy_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_STACK(s);
	RS_ASSERT_PTR(input);
	assert(RS_STACK_CAPACITY >= n);

	memcpy(s->stack.buffer, input, n);
	rs_stack_resize(s, n);
}

RS_API void rs_heap_cpy(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_cpy_n(s, input, strlen(input));
}

RS_API void rs_heap_cpy_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_HEAP(s);
	RS_ASSERT_PTR(input);
	assert(s->heap.capacity >= n);

	memcpy(s->heap.buffer, input, n);
	rs_heap_resize(s, n);
}

RS_API void rs_cpy(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_cpy_n(s, input, strlen(input));
}

RS_API void rs_cpy_n(rapidstring *s, const char *input, size_t n) {
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		rs_grow_heap(s, n);
		rs_heap_cpy_n(s, input, n);
	} else if (RS_HEAP_LIKELY(n > RS_STACK_CAPACITY)) {
		rs_heap_init_g(s, n);
		rs_heap_cpy_n(s, input, n);
	} else {
		rs_stack_cpy_n(s, input, n);
	}
}

RS_API void rs_cpy_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_cpy_n, s, input);
}

/*
 * ===============================================================
 *
 *                         ELEMENT ACCESS
 *
 * ===============================================================
 */

RS_API char *rs_data(rapidstring *s)
{
	return (char*)rs_data_c(s);
}

RS_API const char *rs_data_c(const rapidstring *s)
{
	RS_ASSERT_RS(s);

	return rs_is_heap(s) ?
		s->heap.buffer :
		s->stack.buffer;
}

/*
 * ===============================================================
 *
 *                            CAPACITY
 *
 * ===============================================================
 */

RS_API int rs_empty(const rapidstring *s)
{
	return rs_len(s) == 0;
}

RS_API size_t rs_stack_len(const rapidstring *s)
{
	RS_ASSERT_STACK(s);

	return RS_STACK_CAPACITY - s->stack.left;
}

RS_API size_t rs_heap_len(const rapidstring *s)
{
	RS_ASSERT_HEAP(s);

	return s->heap.size;
}

RS_API size_t rs_len(const rapidstring *s)
{
	return rs_is_heap(s) ?
		rs_heap_len(s) :
		rs_stack_len(s);
}

RS_API size_t rs_capacity(const rapidstring *s)
{
	return rs_is_heap(s) ?
		s->heap.capacity :
		RS_STACK_CAPACITY;
}

RS_API void rs_reserve(rapidstring *s, size_t n)
{
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		if (RS_LIKELY(s->heap.capacity < n))
			rs_realloc(s, n);
	} else {
		rs_stack_to_heap(s, n);
	}
}

RS_API void rs_shrink_to_fit(rapidstring *s)
{
	if (RS_LIKELY(rs_is_heap(s)))
		rs_realloc(s, rs_heap_len(s));
}

RS_API int rs_is_heap(const rapidstring *s)
{
	RS_ASSERT_RS(s);

	return s->heap.flag == RS_HEAP_FLAG;
}

RS_API int rs_is_stack(const rapidstring *s)
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

RS_API void rs_stack_cat(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_stack_cat_n(s, input, strlen(input));
}

RS_API void rs_stack_cat_n(rapidstring *s, const char *input, size_t n)
{
	size_t stack_size;

	RS_ASSERT_PTR(input);
	assert(RS_STACK_CAPACITY >= rs_stack_len(s) + n);

	stack_size = rs_stack_len(s);
	memcpy(s->stack.buffer + stack_size, input, n);
	rs_stack_resize(s, stack_size + n);
}

RS_API void rs_heap_cat(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_heap_cat_n(s, input, strlen(input));
}

RS_API void rs_heap_cat_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_RS(s);
	RS_ASSERT_PTR(input);
	assert(s->heap.capacity >= s->heap.size + n);

	memcpy(s->heap.buffer + rs_heap_len(s), input, n);
	rs_heap_resize(s, rs_heap_len(s) + n);
}

RS_API void rs_cat(rapidstring *s, const char *input)
{
	RS_ASSERT_PTR(input);

	rs_cat_n(s, input, strlen(input));
}

RS_API void rs_cat_n(rapidstring *s, const char *input, size_t n)
{
	if (RS_HEAP_LIKELY(rs_is_heap(s))) {
		rs_grow_heap(s, rs_heap_len(s) + n);
		rs_heap_cat_n(s, input, n);
	} else if (RS_HEAP_LIKELY(s->stack.left < n)) {
		rs_stack_to_heap_g(s, n);
		rs_heap_cat_n(s, input, n);
	} else {
		rs_stack_cat_n(s, input, n);
	}
}

RS_API void rs_cat_rs(rapidstring *s, const rapidstring *input)
{
	RS_DATA_SIZE(rs_cat_n, s, input);
}

RS_API void rs_steal(rapidstring *s, char *buffer)
{
	rs_steal_n(s, buffer, strlen(buffer));
}

RS_API void rs_steal_n(rapidstring *s, char *buffer, size_t n)
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

RS_API void rs_stack_resize(rapidstring *s, size_t n)
{
	assert(RS_STACK_CAPACITY >= n);
	
	s->stack.buffer[n] = '\0';
	s->stack.left = (unsigned char)(RS_STACK_CAPACITY - n);
}

RS_API void rs_heap_resize(rapidstring *s, size_t n)
{
	RS_ASSERT_HEAP(s);
	assert(s->heap.capacity >= n);

	s->heap.buffer[n] = '\0';
	s->heap.size = n;
}

RS_API void rs_resize(rapidstring *s, size_t n)
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

RS_API void rs_resize_w(rapidstring *s, size_t n, char c)
{
	size_t sz = rs_len(s);

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

RS_API void rs_heap_init(rapidstring *s, size_t n)
{
	s->heap.buffer = (char*)RS_ALLOC(n + 1);

	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
	s->heap.flag = RS_HEAP_FLAG;
}

RS_API void rs_heap_init_g(rapidstring *s, size_t n)
{
	rs_heap_init(s, n * RS_GROWTH_FACTOR);
}

RS_API void rs_stack_to_heap(rapidstring *s, size_t n)
{
	const size_t stack_size = rs_stack_len(s);

	char tmp[RS_STACK_CAPACITY];
	memcpy(tmp, s->stack.buffer, stack_size);

	rs_heap_init(s, stack_size + n);
	rs_heap_cpy_n(s, tmp, stack_size);
}

RS_API void rs_stack_to_heap_g(rapidstring *s, size_t n)
{
	rs_stack_to_heap(s, n * RS_GROWTH_FACTOR);
}

RS_API void rs_realloc(rapidstring *s, size_t n)
{
	s->heap.buffer = (char*)RS_REALLOC(s->heap.buffer,
					   s->heap.capacity + 1, n + 1);

	RS_ASSERT_PTR(s->heap.buffer);

	s->heap.capacity = n;
}

RS_API void rs_grow_heap(rapidstring *s, size_t n)
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

RS_API int rsa_stack_can_alloc(size_t n)
{
	const unsigned char *end = rsa_stack.buff + RSA_STACK_SIZE;
	return n <= (size_t)(end - rsa_stack.ptr);
}

RS_API void *rsa_stack_alloc(size_t n)
{
	void *tmp;

	if (RS_UNLIKELY(!rsa_stack_can_alloc(n)))
		return NULL;

	tmp = rsa_stack.ptr;
	rsa_stack.ptr += n;
	return tmp;
}

RS_API void *rsa_stack_realloc(void *p, size_t sz, size_t n)
{
	unsigned char *end;
	int is_end;

	if (RS_UNLIKELY(!p))
		return rsa_stack_alloc(n);

	assert(rsa_stack_owns(p));

	end = rsa_stack.ptr - sz;
	is_end = (unsigned char*)p == end;

	if (RS_LIKELY(is_end && (sz > n || rsa_stack_can_alloc(n - sz)))) {
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

RS_API int rsa_stack_owns(void *p)
{
	unsigned char *ptr;

	RS_ASSERT_PTR(p);

	ptr = (unsigned char*)p;

	return ptr >= rsa_stack.buff &&
	       ptr < rsa_stack.buff + RSA_STACK_SIZE;
}

RS_API void rsa_stack_free(void *p, size_t n)
{
	unsigned char *ptr;

	if (RS_UNLIKELY(!p))
		return;

	ptr = (unsigned char*)p;

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

RS_API void *rsa_alloc(size_t n)
{
	void *p = rsa_stack_alloc(n);

	if (!p)
		return malloc(n);

	return p;
}

RS_API void *rsa_realloc(void *p, size_t sz, size_t n)
{
	if (rsa_stack_owns(p)) {
		void *buff;

		if (rsa_stack_realloc(p, sz, n))
			return p;

		buff = malloc(n);

		if (!buff)
			return NULL;

		memcpy(buff, p, sz);
		rsa_stack_free(p, sz);
		return buff;
	} else {
		return realloc(p, n);
	}
}

RS_API void rsa_free(void *p, size_t n)
{
	if (rsa_stack_owns(p)) {
		rsa_stack_free(p, n);
	} else {
		free(p);
	}
}

#endif /* !RAPID_STRING_H_962AB5F800398A34 */
