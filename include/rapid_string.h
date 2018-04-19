#pragma once

#include <stddef.h>

// TODO: other stl-like stuff?
// TODO: docs & comments
// TODO: some define for validating inputs

#define RS_GCC_INLINE defined(__GNUC__) && __GNUC__ >= 3 && __GNUC_MINOR__ >= 1

#if defined(_MSC_VER) && _MSC_VER >= 1500
#define RS_INLINE __forceinline
#elif GCC_INLINE
#define RS_INLINE __attribute__((always_inline))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define RS_INLINE inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	RS_ERR_SUCCESS,
	RS_ERR_ALLOC
};

/*
 * Accessing packed data structures incurs a performance penalty, therefore the 
 * alignment will be used to allow for a larger stack string.
 */
typedef struct {
	char *buff;
	size_t len;
	size_t cap;
	/*
	 * Bytes will always be lost due to alignment due to the flag. However, 
	 * explicitly storing the flag and the padding inside one of the union 
	 * members allows the stack string to be larger.
	 */
	unsigned char align[sizeof(void*) - 1];
	/*
	 * The following flag is used to store the state of the union as well 
	 * as the remaining cap of the stack string. They may be shared as 
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

extern RS_INLINE rapid_string rs_empty(void);

extern RS_INLINE void rs_init(rapid_string *str);

extern RS_INLINE int rs_init_w(rapid_string *str, const char *input);

extern RS_INLINE int rs_init_w_n(rapid_string *str, const char *input, size_t len);

extern RS_INLINE size_t rs_len(const rapid_string *str);

extern RS_INLINE size_t rs_cap(const rapid_string *str);

extern RS_INLINE int rs_is_heap(const rapid_string *str);

extern RS_INLINE int rs_is_stack(const rapid_string *str);

extern RS_INLINE char rs_at(const rapid_string *str, size_t i);

extern RS_INLINE const char *rs_buff(const rapid_string *str);

extern RS_INLINE int rs_cat(rapid_string *str, const char *input);

extern RS_INLINE int rs_cat_n(rapid_string *str, const char *input, size_t len);

extern RS_INLINE int rs_cpy(rapid_string *str, const char *input);

extern RS_INLINE int rs_cpy_n(rapid_string *str, const char *input, size_t len);

// TODO: explain why buffer must come from malloc / dont use rs_free
extern RS_INLINE void rs_steal(rapid_string *str, char *buffer);

extern RS_INLINE void rs_steal_n(rapid_string *str, char *buffer, size_t cap);

extern RS_INLINE int rs_reserve(rapid_string *str, size_t len);

extern RS_INLINE int rs_resize(rapid_string *str, size_t len);

extern RS_INLINE int rs_shrink_to_fit(rapid_string *str);

extern RS_INLINE void rs_free(rapid_string *str);

#ifdef __cplusplus
}
#endif
