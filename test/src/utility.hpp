#ifndef UTILITY_HPP_ECB97D42D011D625
#define UTILITY_HPP_ECB97D42D011D625

#include "rapidstring.h"
#include <catch.hpp>

#define CMP_STR(s, cmp)                                               \
	do {                                                          \
		REQUIRE(rs_len(s) == cmp.length());                   \
		REQUIRE(rs_data(s) == cmp);                           \
                                                                      \
		if (rs_is_heap(s))                                    \
			REQUIRE(rs_capacity(s) >= cmp.length());      \
		else                                                  \
			REQUIRE(rs_capacity(s) == RS_STACK_CAPACITY); \
	} while (0)

#endif /* !UTILITY_HPP_ECB97D42D011D625 */
