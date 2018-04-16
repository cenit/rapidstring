#include "rapid_str.h"
#include <string.h>
#include <stdlib.h>

#define HEAP_FLAG 0xFF
#define GROWTH_FACTOR 2

static size_t rapid_str_stack_size(const rapid_str *str)
{
	/*
	* The last element in the stack union member stores the remaining
	* capacity. Once the remaining capacity reaches 0, it becomes the null
	* terminator. Credits to Andrei Alexandrescu for the brilliant idea.
	*/
	return sizeof(str->stack) - str->heap.flag;
}

static void rapid_str_cat_heap(rapid_str *str, const char *input, size_t len)
{
	memcpy(str->heap.buffer + str->heap.size, input, len);
	str->heap.size += len;
	str->heap.buffer[str->heap.size] = '\0';
}

static void rapid_str_cat_stack(rapid_str *str, const char *input, size_t len)
{
	memcpy(str->stack + rapid_str_stack_size(str), input, len);
	str->heap.flag -= (unsigned char)len;
	str->stack[rapid_str_stack_size(str)] = '\0';
}

static void rapid_str_copy_heap(rapid_str *str, const char *input, size_t len)
{
	memcpy(str->heap.buffer, input, len);
	str->heap.buffer[len] = '\0';
	str->heap.size = len;
}

static void rapid_str_copy_stack(rapid_str *str, const char *input, size_t len)
{
	memcpy(str->stack, input, len);
	str->stack[len] = '\0';
	str->heap.flag = (unsigned char)(sizeof(str->stack) - len);
}

static int rapid_str_alloc(rapid_str *str, size_t len)
{
	str->heap.buffer = malloc(len + 1);

	if (!str->heap.buffer)
		return RAPID_STR_ERR_ALLOC;

	str->heap.capacity = len;
	str->heap.flag = HEAP_FLAG;

	return 0;
}

static int rapid_str_alloc_cap(rapid_str *str, size_t len) {
	return rapid_str_alloc(str, len * GROWTH_FACTOR);
}

static int rapid_str_realloc_cap(rapid_str *str, size_t len) {
	return rapid_str_realloc(str, len * GROWTH_FACTOR);
}

void rapid_str_init(rapid_str *str) {
	str->stack[0] = '\0';
	str->heap.flag = sizeof(str->stack);
}

int rapid_str_init_w(rapid_str *str, const char *input)
{
	return rapid_str_init_w_n(str, input, strlen(input));
}

int rapid_str_init_w_n(rapid_str *str, const char *input, size_t len)
{
	rapid_str_init(str);
	return rapid_str_copy_n(str, input, len);
}

size_t rapid_str_size(const rapid_str *str)
{
	return str->heap.flag == HEAP_FLAG ? 
		str->heap.size :
		rapid_str_stack_size(str);
}

size_t rapid_str_capacity(const rapid_str *str)
{
	return str->heap.flag == HEAP_FLAG ? 
		str->heap.capacity :
		sizeof(str->stack);
}

char rapid_str_at(const rapid_str *str, size_t i)
{
	return rapid_str_buff(str)[i];
}

const char *rapid_str_buff(const rapid_str *str)
{
	return str->heap.flag == HEAP_FLAG ? str->heap.buffer : str->stack;
}

int rapid_str_cat(rapid_str *str, const char *input)
{
	return rapid_str_cat_n(str, input, strlen(input));
}

int rapid_str_cat_n(rapid_str *str, const char *input, size_t len)
{
	if (str->heap.flag == HEAP_FLAG) {
		int rc = rapid_str_realloc_cap(str, str->heap.capacity + len);

		if (rc)
			return rc;

		rapid_str_cat_heap(str, input, len);
	} else if (str->heap.flag <= len) {
		int rc = rapid_str_alloc_cap(str, len);

		if (rc)
			return rc;

		rapid_str_cat_heap(str, input, len);
	} else {
		rapid_str_cat_stack(str, input, len);
	}

	return 0;
}

int rapid_str_copy(rapid_str *str, const char *input)
{
	return rapid_str_copy_n(str, input, strlen(input));
}

int rapid_str_copy_n(rapid_str *str, const char *input, size_t len) {
	if (str->heap.flag == HEAP_FLAG) {
		int rc = rapid_str_realloc_cap(str, len);

		if (rc)
			return rc;

		rapid_str_copy_heap(str, input, len);
	} else if (len >= sizeof(str->stack)) {
		int rc = rapid_str_alloc_cap(str, len);

		if (rc)
			return rc;

		rapid_str_copy_heap(str, input, len);
	} else {
		rapid_str_copy_stack(str, input, len);
	}

	return 0;
}

void rapid_str_steal(rapid_str *str, char *buffer)
{
	rapid_str_steal_n(&str, buffer, strlen(buffer));
}

void rapid_str_steal_n(rapid_str *str, char *buffer, size_t len)
{
	str->heap.buffer = buffer;
	str->heap.size = len;
	str->heap.capacity = len;
}

int rapid_str_realloc(rapid_str *str, size_t len) {
	char *new_buff = realloc(str->heap.buffer, len + 1);

	if (!new_buff)
		return RAPID_STR_ERR_ALLOC;

	str->heap.buffer = new_buff;
	str->heap.capacity = len;

	return 0;
}

int rapid_str_shrink_to_fit(rapid_str *str)
{
	if (str->heap.flag != HEAP_FLAG)
		return 0;

	return rapid_str_realloc(str, rapid_str_size(str));
}

void rapid_str_free(rapid_str *str)
{
	if (str->heap.flag == HEAP_FLAG)
		free(str->heap.buffer);

	rapid_str_init(str);;
}
