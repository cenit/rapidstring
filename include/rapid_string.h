#ifndef RAPID_STRING_H_962AB5F800398A34
#define RAPID_STRING_H_962AB5F800398A34

#include <stddef.h>

/* TODO: other stl-like stuff? */
/* TODO: docs & comments */
/* TODO: some define for validating inputs */

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
	#define RS_GCC_VERSION (__GNUC__       * 10000 + \
				__GNUC_MINOR__ *   100 + \
				__GNUC_PATCHLEVEL__)
#else
	#define RS_GCC_VERSION (0)
#endif

#ifdef __STDC_VERSION__
#define RS_C99 (__STDC_VERSION__ >= 199901L)
#define RS_C11 (__STDC_VERSION__ >= 201112L)
#else
#define RS_C99 (0)
#define RC_C11 (0)
#endif

#if (defined(_MSC_VER) && _MSC_VER >= 1500) || defined (__INTEL_COMPILER)
	#define RS_INLINE __forceinline
#elif RS_GCC_VERSION >= 30100
	#define RS_INLINE __attribute__((always_inline))
#elif RS_C99
	#define RS_INLINE inline
#else
	#define RS_INLINE
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
	 * The following flag is used to store the state of the union as well 
	 * as the remaining capacity of the stack string. They may be shared as 
	 * said cap of a stack string will always be smaller than the heap 
	 * flag, which guarentees unambiguity between the two states.
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

extern RS_INLINE rapid_string
rs_empty(void);

extern RS_INLINE void
rs_init(rapid_string *str);

extern RS_INLINE int
rs_init_w(rapid_string *str, const char *input);

extern RS_INLINE int
rs_init_w_n(rapid_string *str, const char *input, size_t len);

extern RS_INLINE size_t
rs_len(const rapid_string *str);

extern RS_INLINE size_t
rs_cap(const rapid_string *str);

extern RS_INLINE int
rs_is_heap(const rapid_string *str);

extern RS_INLINE int
rs_is_stack(const rapid_string *str);

extern RS_INLINE char
rs_at(const rapid_string *str, size_t i);

extern RS_INLINE const char*
rs_buff(const rapid_string *str);

extern RS_INLINE int
rs_cat(rapid_string *str, const char *input);

extern RS_INLINE int
rs_cat_n(rapid_string *str, const char *input, size_t len);

extern RS_INLINE int
rs_cpy(rapid_string *str, const char *input);

extern RS_INLINE int
rs_cpy_n(rapid_string *str, const char *input, size_t len);

extern RS_INLINE void
rs_steal(rapid_string *str, char *buffer);

extern RS_INLINE void
rs_steal_n(rapid_string *str, char *buffer, size_t cap);

extern RS_INLINE int
rs_reserve(rapid_string *str, size_t len);

extern RS_INLINE int
rs_resize(rapid_string *str, size_t len);

extern RS_INLINE int
rs_shrink_to_fit(rapid_string *str);

extern RS_INLINE void
rs_free(rapid_string *str);

#ifdef __cplusplus
}
#endif

#endif /* !RAPID_STRING_H_962AB5F800398A34 */
