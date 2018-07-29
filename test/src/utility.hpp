#ifndef UTILITY_HPP_ECB97D42D011D625
#define UTILITY_HPP_ECB97D42D011D625

#include "rapidstring.h"
#include <catch.hpp>
#include <string>

#define VALIDATE_RS(s, cmp)                                      \
	do {                                                     \
		const auto cmp_str = cmp;                        \
		REQUIRE(rs_len(s) == cmp_str.length());          \
		REQUIRE(rs_data_c(s)[rs_len(s)] == '\0');        \
		REQUIRE(rs_data_c(s) == cmp_str);                \
                                                                 \
		if (rs_is_heap(s))                               \
			REQUIRE(rs_cap(s) >= cmp_str.length());  \
		else                                             \
			REQUIRE(rs_cap(s) == RS_STACK_CAPACITY); \
	} while (0)

#endif /* !UTILITY_HPP_ECB97D42D011D625 */
