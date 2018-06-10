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
 * - Declarations:	line 79
 *
 * 2. CONSTRUCTION & DESTRUCTION
 * - Declarations:	line 375
 * - Defintions:	line 1147
 *
 * 3. COPYING
 * - Declarations:	line 483
 * - Defintions:	line 1195
 *
 * 4. CAPACITY
 * - Declarations:	line 632
 * - Defintions:	line 1262
 *
 * 5. MODIFIERS
 * - Declarations:	line 771
 * - Defintions:	line 1327
 *
 * 6. HEAP OPERATIONS
 * - Declarations:	line 1040
 * - Defintions:	line 1477
 */

/**
 * @file rapidstring.h
 * @brief The single header of the rapidstring library.
 */

/**
 * @mainpage rapidstring
 * The documentation of the rapidstring library.
 *
 * @copyright Copyright © 2018 John Boyer.
 * @author <a href="https://github.com/boyerjohn">John Boyer</a>
 * @version 0.1.0
 */

#ifndef RAPIDSTRING_H_962AB5F800398A34
#define RAPIDSTRING_H_962AB5F800398A34

/**
 * @todo Make sure all std::string methods are added (if applicable).
 *
 * @todo Create rs_search, rs_erase, rs_substring.
 *
 * @todo Generate man pages and install them.
 */

#include <assert.h> /* assert() */
#include <string.h> /* memcpy() */

/*
 * ===============================================================
 *
 *                       STRUCTURES & MACROS
 *
 * ===============================================================
 */

/**
 * @defgroup structures Structures & macros
 * Structure and macro definitions.
 * @{
 */

/**
 * @brief Major version of the rapidstring library.
 *
 * @since 1.0.0
 */
#define RS_VERSION_MAJOR 0

/**
 * @brief Minor version of the rapidstring library.
 *
 * @since 1.0.0
 */
#define RS_VERSION_MINOR 1

/**
 * @brief Patch version of the rapidstring library.
 *
 * @since 1.0.0
 */
#define RS_VERSION_PATCH 0

#ifndef RS_GROWTH_FACTOR
/**
 * @brief String growth factor macro.
 *
 * A string's capacity will be increased by this factor every time it runs out
 * of space. Redefine this macro depending on your application's needs.
 *
 * @since 1.0.0
 */
#define RS_GROWTH_FACTOR (2)
#endif

#ifndef RS_AVERAGE_SIZE
/**
 * @brief Average string size macro.
 *
 * This macro is used to help the branch predictor. If you specify that your
 * application's average string size is `8`, the branches will be optimized
 * for stack strings. The same would occur for heap strings if a large size
 * is provided.
 *
 * @since 1.0.0
 */
#define RS_AVERAGE_SIZE (20)
#endif

#if !defined(RS_MALLOC) && !defined(RS_REALLOC) && !defined(RS_FREE)
#include <stdlib.h>

/**
 * @brief Allocation macro.
 *
 * If this macro is redefined, RS_REALLOC() and RS_FREE() must be aswell.
 *
 * @since 1.0.0
 */
#define RS_MALLOC malloc

/**
 * @brief Reallocation macro.
 *
 * If this macro is redefined, RS_MALLOC() and RS_FREE() must be aswell.
 *
 * @since 1.0.0
 */
#define RS_REALLOC realloc

/**
 * @brief Deallocation macro.
 *
 * If this macro is redefined, RS_MALLOC() and RS_REALLOC() must be aswell.
 *
 * @since 1.0.0
 */
#define RS_FREE free
#endif

/**
 * @brief Heap flag of a #rapidstring.
 *
 * @warning Intended for internal use.
 *
 * @since 1.0.0
 */
#define RS_HEAP_FLAG (0xFF)

#define RS_ASSERT_RS(s)                                    \
	do {                                               \
		assert(s != NULL);                         \
		assert(s->heap.flag == RS_HEAP_FLAG ||     \
		       s->heap.flag <= RS_STACK_CAPACITY); \
	} while (0)

#ifdef __GNUC__
#define RS_GCC_VERSION \
	(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define RS_GCC_VERSION (0)
#endif

/* GCC version 2.96 required for branch prediction expectation. */
#if RS_GCC_VERSION > 29600
#define RS_EXPECT(expr, val) __builtin_expect(expr, val)
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

#ifdef RS_NOINLINE
/* GCC version 3.1 required for the no inline attribute. */
#if RS_GCC_VERSION > 30100
#define RS_API static __attribute__((noinline))
#elif defined(_MSC_VER)
#define RS_API static __declspec(noinline)
#else
#define RS_API static
#endif
#elif RS_C99
#define RS_API static inline
#elif defined(__GNUC__)
#define RS_API static __inline__
#elif defined(_MSC_VER)
#define RS_API static __forceinline
#else
#define RS_API static
#endif

typedef struct {
	void *pointer;
	size_t size;
} rs_align_dummy;

#if RS_C11
#define RS_ALIGNMENT (_Alignof(rs_align_dummy))
#elif defined(__GNUC__)
#define RS_ALIGNMENT (__alignof__(rs_align_dummy))
#elif defined(_MSC_VER)
#define RS_ALIGNMENT (__alignof(rs_align_dummy))
#else
/*
 * No other way to find the alignment than assuming structs are aligned
 * by the largest member.
 */
#define RS_ALIGNMENT \
	(sizeof(void *) > sizeof(size_t) ? sizeof(void *) : sizeof(size_t))
#endif

/**
 * @brief Struct that stores the heap data.
 *
 * Accessing packed data structures incurs a performance penalty, therefore the
 * alignment will be used to allow for a larger stack string.
 *
 * @warning Intended for internal use.
 *
 * @since 1.0.0
 */
typedef struct {
	/**
	 * @brief Buffer of a heap string.
	 *
	 * Allocated using RS_MALLOC() or RS_REALLOC(). This buffer may be
	 * manually freed by directly calling `RS_FREE(s->heap.buffer)`. Doing
	 * so will avoid the heap flag check.
	 */
	char *buffer;
	/**
	 * @brief Number of characters in a heap string.
	 *
	 * The null terminator is not included.
	 */
	size_t size;
	/**
	 * @brief Capacity of a heap string.
	 *
	 * The null terminator is not included.
	 */
	size_t capacity;
	/**
	 * @brief Alignnment of a heap string.
	 *
	 * Ensures @a flag and @a left are stored in the same location.
	 */
	unsigned char align[RS_ALIGNMENT - 1];
	/**
	 * @brief Flag of the rapidstring union.
	 *
	 * The following is used to store the state of the union as well as the
	 * remaining capacity of the stack string. They may be shared as the
	 * capacity of a stack string will always be smaller than
	 * #RS_HEAP_FLAG, which guarentees unambiguity between the two states.
	 */
	unsigned char flag;
} rs_heap;

/**
 * @brief Capacity of a stack string.
 *
 * @since 1.0.0
 */
#define RS_STACK_CAPACITY (sizeof(rs_heap) - 1)

/**
 * @brief Struct that stores the stack data.
 *
 * @warning Intended for internal use.
 *
 * @since 1.0.0
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
	 * Once the stack string runs out of space, zero will be written to
	 * this member, effectively becoming the null terminator.
	 */
	unsigned char left;
} rs_stack;

/**
 * @brief Union that stores a #rapidstring.
 *
 * Some API functions are prefixed with `rs_heap_x()` or `rs_stack_x()`. These
 * functions are to be used only when a string is guarenteed to be in either
 * state, as mismatching these functions results in undefined behavior. There
 * will always be the `rs_x()` alternative which automatically handles the state
 * of the string. These functions should be used if optimization isn't a
 * necessity or if strings have highly variable sizes.
 *
 * @since 1.0.0
 */
typedef union {
	/** @brief Stack state of the #rapidstring union. */
	rs_stack stack;
	/** @brief Heap state of the #rapidstring union. */
	rs_heap heap;
} rapidstring;

/* Based off the average string size, allow for more efficient branching. */
enum { RS_HEAP_LIKELY_V = RS_AVERAGE_SIZE > RS_STACK_CAPACITY };

#define RS_HEAP_LIKELY(expr) RS_EXPECT(expr, RS_HEAP_LIKELY_V)
#define RS_STACK_LIKELY(expr) RS_EXPECT(expr, !RS_HEAP_LIKELY_V)

/**
 * @brief Pass the string data and size to a function.
 *
 * Forwards the buffer and size of a string to the provided function.
 * Retrieving both the buffer and the size of a string requires a flag check,
 * which would result in an additional branch if not done manually.
 *
 * @param[in] f A function.
 * @param[in,out] s An initialized string.
 * @param[in] input The input to forward the function.
 *
 * @warning Intended for internal use.
 *
 * @since 1.0.0
 */
#define RS_DATA_SIZE(f, s, input)                                       \
	do {                                                            \
		if (RS_HEAP_LIKELY(rs_is_heap(input)))                  \
			f(s, input->heap.buffer, rs_heap_len(input));   \
		else                                                    \
			f(s, input->stack.buffer, rs_stack_len(input)); \
	} while (0)

/** @} */

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */

/**
 * @defgroup construction Construction & destruction
 * Functions that initialize and free strings.
 * @{
 */

/**
 * @brief Initializes a string.
 *
 * @param[out] s The string to initialize.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API void rs_init(rapidstring *s);

/**
 * @brief Initializes a string with a character array.
 *
 * @param[out] s A string to initialize.
 * @param[in] input The input used to initialize the string.
 *
 * @note Identicle to rs_init_w_n() with `strlen()`.
 *
 * @allocation When the legnth of @a input is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_init_w(rapidstring *s, const char *input);

/**
 * @brief Initializes a string with a character array.
 *
 * @param[out] s A string to initialize.
 * @param[in] input The input used to initialize the string.
 * @param[in] n The length of the input.
 *
 * @allocation When @a n is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_init_w_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Initializes a string with an initial capacity.
 *
 * @param[out] s A string to initialize.
 * @param[in] n The new initial capacity of the string.
 *
 * @allocation Always.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_init_w_cap(rapidstring *s, size_t n);

/**
 * @brief Initializes a string with another string.
 *
 * @param[out] s A string to initialize.
 * @param[in] input The input string used to initialize @a s.
 *
 * @allocation When the length of @a input is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_init_w_rs(rapidstring *s, const rapidstring *input);

/**
 * @brief Frees a string.
 *
 * The string is in an invalid state after freeing. You must call rs_init() if
 * you wish to reuse the same string.
 *
 * A jump may be avoided by directly calling `RS_FREE(s->heap.buffer)` if the
 * string is known to be on the heap.
 *
 * Calling this fuction is unnecessary if the string size is always smaller or
 * equal to #RS_STACK_CAPACITY.
 *
 * @param[in] s The string to free.
 *
 * @allocation Never.
 *
 * @since 1.0.0
 */
RS_API void rs_free(rapidstring *s);

/** @} */

/*
 * ===============================================================
 *
 *                             COPYING
 *
 * ===============================================================
 */

/**
 * @defgroup copying Copying
 * Functions that copy data into a string.
 * @{
 */

/**
 * @brief Copies characters to a stack string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An intialized stack string.
 * @param[in] input the input to assign to the stack string.
 *
 * @warning The input length must be smaller or equal to #RS_STACK_CAPACITY. If
 * this is inconvenient for your usage, use rs_cpy().
 *
 * @note Identicle to rs_stack_cpy_n() with `strlen()`.
 *
 * @allocation Never.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_cpy(rapidstring *s, const char *input);

/**
 * @brief Copies characters to a stack string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to assign to the stack string.
 * @param[in] n The length of @a input.
 *
 * @warning The input length must be smaller or equal to #RS_STACK_CAPACITY. If
 * this is inconvenient for your usage, use rs_cpy().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Copies characters to a heap string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to assign to the heap string.
 *
 * @warning The input length must be smaller or equal to the capacity of @a s.
 * If this is inconvenient for your usage, use rs_cpy().
 *
 * @note Identicle to rs_heap_cpy_n() with `strlen()`.
 *
 * @allocation Never.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_cpy(rapidstring *s, const char *input);

/**
 * @brief Copies characters to a heap string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to assign to the heap string.
 * @param[in] n The length of @a input.
 *
 * @warning The input length must be smaller or equal to the capacity of @a s.
 * If this is inconvenient for your usage, use rs_cpy().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Copies characters to a string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized string.
 * @param[in] input The input to assign to the string.
 *
 * @note Identicle to rs_cpy_n() with `strlen()`.
 *
 * @allocation When the length of @a input is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_cpy(rapidstring *s, const char *input);

/**
 * @brief Copies characters to a string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized string.
 * @param[in] input The input to assign to the string.
 * @param[in] n The length of @a input.
 *
 * @allocation When @a n is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_cpy_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Copies characters from a string to another string.
 *
 * Overwrites any existing data.
 *
 * @param[in,out] s An initialized string.
 * @param[in] input The input string used to initialize @a s.
 *
 * @allocation When the length of @a input is greater than #RS_STACK_CAPACITY.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_cpy_rs(rapidstring *s, const rapidstring *input);

/** @} */

/*
 * ===============================================================
 *
 *                            CAPACITY
 *
 * ===============================================================
 */

/**
 * @defgroup capacity Capacity
 * Functions related to the capacity of a string.
 * @{
 */

/**
 * @brief Checks whether a string is empty.
 *
 * @param[in] s An initialized string.
 * @returns `1` if the string is empty, `0` otherwise.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API unsigned char rs_empty(const rapidstring *s);

/**
 * @brief Returns the length of a stack string.
 *
 * @param[in] s An initialized stack string.
 * @returns The stack string length.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API size_t rs_stack_len(const rapidstring *s);

/**
 * @brief Returns the length of a heap string.
 *
 * @param[in] s An initialized heap string.
 * @returns The heap string length.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API size_t rs_heap_len(const rapidstring *s);

/**
 * @brief Returns the length.
 *
 * @param[in] s An initialized string.
 * @returns The string length.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API size_t rs_len(const rapidstring *s);

/**
 * @brief Returns the capacity.
 *
 * @param[in] s An initialized string.
 * @returns The string capacity.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API size_t rs_capacity(const rapidstring *s);

/**
 * @brief Reserves capacity.
 *
 * @param[in,out] s An initialized string.
 * @param[in] n The capacity to reserve.
 *
 * @allocation Always.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_reserve(rapidstring *s, size_t n);

/**
 * @brief Frees all unused memory.
 *
 * @param[in,out] s An intialized string.
 *
 * @allocation Never.
 *
 * @since 1.0.0
 */
RS_API void rs_shrink_to_fit(rapidstring *s);

/**
 * @brief Checks whether a string is on the heap.
 *
 * @param[in] s An initialized string.
 * @returns `1` if the string is on the heap, `0` otherwise.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API unsigned char rs_is_heap(const rapidstring *s);

/**
 * @brief Checks whether a string is on the stack.
 *
 * @param[in] s An initialized string.
 * @returns `1` if the string is on the stack, `0` otherwise.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API unsigned char rs_is_stack(const rapidstring *s);

/** @} */

/*
 * ===============================================================
 *
 *                            MODIFIERS
 *
 * ===============================================================
 */

/**
 * @defgroup modifiers Modifiers
 * Modifier functions such as concatenation, stealing and resizing.
 * @{
 */

/**
 * @brief Access the buffer.
 *
 * @param[in] s An initialized string.
 * @returns The buffer.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API char *rs_data(rapidstring *s);

/**
 * @brief Access the readonly buffer.
 *
 * @param[in] s An initialized string.
 * @returns The readonly buffer.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API const char *rs_data_c(const rapidstring *s);

/**
 * @brief Concatenates characters to a stack string.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to concatenate.
 *
 * @warning The input length must be smaller or equal to the remaining capacity
 * of @a s. If this is inconvenient for your usage, use rs_cat().
 *
 * @note Identicle to rs_stack_cat_n() with `strlen()`.
 *
 * @allocation Never.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_cat(rapidstring *s, const char *input);

/**
 * @brief Concatenates characters to a stack string.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] input The input to concatenate.
 * @param[in] n The length of @a input.
 *
 * @warning The input length must be smaller or equal to the remaining capacity
 * of @a s. If this is inconvenient for your usage, use rs_cat().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Concatenates characters to a heap string.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to concatenate.
 *
 * @warning The input length must be smaller or equal to the remaining capacity
 * of @a s. If this is inconvenient for your usage, use rs_cat().
 *
 * @note Identicle to rs_heap_cat_n() with `strlen()`.
 *
 * @allocation Never.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_cat(rapidstring *s, const char *input);

/**
 * @brief Concatenates characters to a heap string.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] input The input to concatenate.
 * @param[in] n The length of @a input.
 *
 * @warning The input length must be smaller or equal to the remaining capacity
 * of @a s. If this is inconvenient for your usage, use rs_cat().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Concatenates characters to a string.
 *
 * @param[in,out] s An initialized  string.
 * @param[in] input The input to concatenate.
 *
 * @note Identicle to rs_cat_n() with `strlen()`.
 *
 * @allocation When the length of @a input is greater than the remaining
 * capacity of @a s.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_cat(rapidstring *s, const char *input);

/**
 * @brief Concatenates characters to a string.
 *
 * @param[in,out] s An initialized  string.
 * @param[in] input The input to concatenate.
 * @param[in] n The length of @a input.
 *
 * @allocation When @a n is greater than the remaining capacity of @a s.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_cat_n(rapidstring *s, const char *input, size_t n);

/**
 * @brief Concatenates a string to another string.
 *
 * @param[in,out] s An initialized string.
 * @param[in] input The input to concatenate.
 *
 * @allocation When the length of @a input is greater than the remaining
 * capacity of @a s.
 *
 * @complexity Linear in the length of @a input.
 *
 * @since 1.0.0
 */
RS_API void rs_cat_rs(rapidstring *s, const rapidstring *input);

/**
 * @brief Steals a buffer allocated on the heap.
 *
 * The buffer must either be allocated with RS_MALLOC() or RS_REALLOC(), or it
 * must be manually freed.
 *
 * @param[in,out] s An initialized string.
 * @param[in] buffer The buffer to steal.
 *
 * @note Identicle to rs_steal_n() with `strlen() + 1`.
 *
 * @allocation Never.
 *
 * @complexity Linear in the length of @a buffer.
 *
 * @since 1.0.0
 */
RS_API void rs_steal(rapidstring *s, char *buffer);

/**
 * @brief Steals a buffer allocated on the heap.
 *
 * The buffer must either be allocated with RS_MALLOC() or RS_REALLOC(), or it
 * must be manually freed.
 *
 * @param[in,out] s An initialized string.
 * @param[in] buffer The buffer to steal.
 * @param[in] cap The capacity of @a buffer.
 *
 * @allocation Never.
 *
 * @complexity Constant.
 *
 * @since 1.0.0
 */
RS_API void rs_steal_n(rapidstring *s, char *buffer, size_t cap);

/**
 * @brief Resizes a stack string.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] n The new size.
 *
 * @warning The new size must be smaller or equal to #RS_STACK_CAPACITY. If this
 * is inconvenient for your usage, use rs_resize().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_resize(rapidstring *s, size_t n);

/**
 * @brief Resizes a heap string.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new size.
 *
 * @warning The new size must be smaller or equal to the capacity of @a s. If
 * this is inconvenient for your usage, use rs_resize().
 *
 * @allocation Never.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_resize(rapidstring *s, size_t n);

/**
 * @brief Resizes a string.
 *
 * @param[in,out] s An initialized string.
 * @param[in] n The new size.
 *
 * @warning If the string size increases, the new characters will not be
 * initialized. This will leave the end of your string with garbage. If this is
 * inconvenient for your usage, use rs_resize_w().
 *
 * @allocation When @a n is greater than the capacity of @a s.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_resize(rapidstring *s, size_t n);

/**
 * @brief Resizes a string with a filler character.
 *
 * @param[in,out] s An initialized string.
 * @param[in] n The new size.
 * @param[in] c The filler character.
 *
 * @allocation When @a n is greater than the capacity of @a s.
 *
 * @complexity Linear in @a n.
 *
 * @since 1.0.0
 */
RS_API void rs_resize_w(rapidstring *s, size_t n, char c);

/** @} */

/*
 * ===============================================================
 *
 *                         HEAP OPERATIONS
 *
 * ===============================================================
 */

/**
 * @defgroup heap Heap operations
 * Heap functions intended for internal use.
 * @{
 */

/**
 * @brief Initializes the heap.
 *
 * @param[out] s A string to initialize.
 * @param[in] n The heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @allocation Always.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_init(rapidstring *s, size_t n);

/**
 * @brief Initializes the heap with growth.
 *
 * @param[out] s A string to initialize.
 * @param[in] n The heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @note Identicle to `rs_heap_init(s, n * RS_GROWTH_FACTOR)`.
 *
 * @allocation Always.
 *
 * @since 1.0.0
 */
RS_API void rs_heap_init_g(rapidstring *s, size_t n);

/**
 * @brief Moves a stack string to the heap.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] n The heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @allocation Always.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_to_heap(rapidstring *s, size_t n);

/**
 * @brief Move a stack string to the heap with growth.
 *
 * @param[in,out] s An initialized stack string.
 * @param[in] n The heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @note Identicle to `rs_stack_to_heap(s, n * RS_GROWTH_FACTOR)`.
 *
 * @allocation Always.
 *
 * @since 1.0.0
 */
RS_API void rs_stack_to_heap_g(rapidstring *s, size_t n);

/**
 * @brief Reallocates the heap buffer.
 *
 * Grows or shrinks the heap capacity. The size will remain the same, even if
 * the new capacity is smaller than the current size.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @allocation Always.
 *
 * @since 1.0.0
 */
RS_API void rs_realloc(rapidstring *s, size_t n);

/**
 * @brief Allocates growth for a heap string.
 *
 * @param[in,out] s An initialized heap string.
 * @param[in] n The new heap capacity.
 *
 * @warning Intended for internal use.
 *
 * @allocation When @a n is greater than the capacity of @a s.
 *
 * @since 1.0.0
 */
RS_API void rs_grow_heap(rapidstring *s, size_t n);

/** @} */

/*
 * ===============================================================
 *
 *                   CONSTRUCTION & DESTRUCTION
 *
 * ===============================================================
 */

RS_API void rs_init(rapidstring *s)
{
	assert(s != NULL);

	s->stack.buffer[0] = '\0';
	s->stack.left = RS_STACK_CAPACITY;
}

RS_API void rs_init_w(rapidstring *s, const char *input)
{
	assert(input != NULL);

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
		RS_FREE(s->heap.buffer);
}

/*
 * ===============================================================
 *
 *                             COPYING
 *
 * ===============================================================
 */

RS_API void rs_stack_cpy(rapidstring *s, const char *input)
{
	assert(s != NULL);

	rs_stack_cpy_n(s, input, strlen(input));
}

RS_API void rs_stack_cpy_n(rapidstring *s, const char *input, size_t n)
{
	assert(rs_is_stack(s));
	assert(input != NULL);
	assert(RS_STACK_CAPACITY >= n);

	memcpy(s->stack.buffer, input, n);
	rs_stack_resize(s, n);
}

RS_API void rs_heap_cpy(rapidstring *s, const char *input)
{
	assert(input != NULL);

	rs_heap_cpy_n(s, input, strlen(input));
}

RS_API void rs_heap_cpy_n(rapidstring *s, const char *input, size_t n)
{
	assert(rs_is_heap(s));
	assert(input != NULL);
	assert(s->heap.capacity >= n);

	memcpy(s->heap.buffer, input, n);
	rs_heap_resize(s, n);
}

RS_API void rs_cpy(rapidstring *s, const char *input)
{
	assert(input != NULL);

	rs_cpy_n(s, input, strlen(input));
}

RS_API void rs_cpy_n(rapidstring *s, const char *input, size_t n)
{
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
 *                            CAPACITY
 *
 * ===============================================================
 */

RS_API unsigned char rs_empty(const rapidstring *s)
{
	return rs_len(s) == 0;
}

RS_API size_t rs_stack_len(const rapidstring *s)
{
	assert(rs_is_stack(s));

	return RS_STACK_CAPACITY - s->stack.left;
}

RS_API size_t rs_heap_len(const rapidstring *s)
{
	assert(rs_is_heap(s));

	return s->heap.size;
}

RS_API size_t rs_len(const rapidstring *s)
{
	return rs_is_heap(s) ? rs_heap_len(s) : rs_stack_len(s);
}

RS_API size_t rs_capacity(const rapidstring *s)
{
	return rs_is_heap(s) ? s->heap.capacity : RS_STACK_CAPACITY;
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

RS_API unsigned char rs_is_heap(const rapidstring *s)
{
	RS_ASSERT_RS(s);

	return s->heap.flag == RS_HEAP_FLAG;
}

RS_API unsigned char rs_is_stack(const rapidstring *s)
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

RS_API char *rs_data(rapidstring *s)
{
	return (char *)rs_data_c(s);
}

RS_API const char *rs_data_c(const rapidstring *s)
{
	RS_ASSERT_RS(s);

	return rs_is_heap(s) ? s->heap.buffer : s->stack.buffer;
}

RS_API void rs_stack_cat(rapidstring *s, const char *input)
{
	assert(input != NULL);

	rs_stack_cat_n(s, input, strlen(input));
}

RS_API void rs_stack_cat_n(rapidstring *s, const char *input, size_t n)
{
	size_t stack_size;

	assert(input != NULL);
	assert(RS_STACK_CAPACITY >= rs_stack_len(s) + n);

	stack_size = rs_stack_len(s);
	memcpy(s->stack.buffer + stack_size, input, n);
	rs_stack_resize(s, stack_size + n);
}

RS_API void rs_heap_cat(rapidstring *s, const char *input)
{
	assert(s != NULL);

	rs_heap_cat_n(s, input, strlen(input));
}

RS_API void rs_heap_cat_n(rapidstring *s, const char *input, size_t n)
{
	RS_ASSERT_RS(s);
	assert(input != NULL);
	assert(s->heap.capacity >= s->heap.size + n);

	memcpy(s->heap.buffer + rs_heap_len(s), input, n);
	rs_heap_resize(s, rs_heap_len(s) + n);
}

RS_API void rs_cat(rapidstring *s, const char *input)
{
	assert(input != NULL);

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
	assert(buffer != NULL);

	rs_steal_n(s, buffer, strlen(buffer) + 1);
}

RS_API void rs_steal_n(rapidstring *s, char *buffer, size_t n)
{
	size_t sz;

	/* Manual free as using rs_free creates an additional branch. */
	if (RS_HEAP_LIKELY(rs_is_heap(s)))
		RS_FREE(s->heap.buffer);
	else
		s->heap.flag = RS_HEAP_FLAG;

	s->heap.buffer = buffer;

	/* The capacity and size do not include the null terminator. */
	sz = n - 1;

	s->heap.capacity = sz;
	rs_heap_resize(s, sz);
}

RS_API void rs_stack_resize(rapidstring *s, size_t n)
{
	assert(RS_STACK_CAPACITY >= n);

	s->stack.buffer[n] = '\0';
	s->stack.left = (unsigned char)(RS_STACK_CAPACITY - n);
}

RS_API void rs_heap_resize(rapidstring *s, size_t n)
{
	assert(rs_is_heap(s));
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
			rs_stack_to_heap(s, n);

		rs_heap_resize(s, n);
	} else {
		rs_stack_resize(s, n);
	}
}

RS_API void rs_resize_w(rapidstring *s, size_t n, char c)
{
	const size_t sz = rs_len(s);

	rs_resize(s, n);

	if (RS_LIKELY(sz < n)) {
		const size_t diff = n - sz;
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
	s->heap.buffer = (char *)RS_MALLOC(n + 1);
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
	s->heap.buffer = (char *)RS_REALLOC(s->heap.buffer, n + 1);
	s->heap.capacity = n;
}

RS_API void rs_grow_heap(rapidstring *s, size_t n)
{
	if (RS_UNLIKELY(s->heap.capacity < n))
		rs_realloc(s, n * RS_GROWTH_FACTOR);
}

#endif /* !RAPIDSTRING_H_962AB5F800398A34 */
