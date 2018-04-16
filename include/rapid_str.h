#pragma once

#include <stddef.h>

// TODO: other stl-like stuff?
// TODO: docs & comments

enum {
	RAPID_STR_ERR_SUCCESS,
	RAPID_STR_ERR_ALLOC
};

#define RAPID_STR_ALIGNMENT \
	sizeof(void*) > sizeof(size_t) ? sizeof(void*) : sizeof(size_t)

typedef struct {
	char *buffer;
	size_t size;
	size_t capacity;
	/*
	 * Bytes will always be lost due to alignment due to the flag. However, 
	 * explicitly storing the flag and the padding inside one of the union 
	 * members allows the stack string to be larger.
	 */
	unsigned char align[RAPID_STR_ALIGNMENT - 1];
	/*
	 * The following flag is used to store the state of the union as well 
	 * as the remaining capacity of the stack string. They may be shared as 
	 * said capacity of a stack string will always be smaller than the heap 
	 * flag, which guarentees unambiguity between the two states.
	 */
	unsigned char flag;
} rapid_str_heap;

typedef union {
	char stack[sizeof(rapid_str_heap)];
	rapid_str_heap heap;
} rapid_str;

void rapid_str_init(rapid_str *str);

int rapid_str_init_w(rapid_str *str, const char *input);

int rapid_str_init_w_n(rapid_str *str, const char *input, size_t len);

size_t rapid_str_size(const rapid_str *str);

size_t rapid_str_capacity(const rapid_str *str);

char rapid_str_at(const rapid_str *str, size_t i);

const char *rapid_str_buff(const rapid_str *str);

int rapid_str_cat(rapid_str *str, const char *input);

int rapid_str_cat_n(rapid_str *str, const char *input, size_t len);

int rapid_str_copy(rapid_str *str, const char *input);

int rapid_str_copy_n(rapid_str *str, const char *input, size_t len);

// TODO: explain why buffer must come from malloc / dont use rapid_str_free
void rapid_str_steal(rapid_str *str, char *buffer);

void rapid_str_steal_n(rapid_str *str, char *buffer, size_t len);

int rapid_str_realloc(rapid_str *str, size_t len);

int rapid_str_shrink_to_fit(rapid_str *str);

void rapid_str_free(rapid_str *str);
