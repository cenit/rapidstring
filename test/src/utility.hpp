#ifndef UTILITY_HPP_ECB97D42D011D625
#define UTILITY_HPP_ECB97D42D011D625

#include "rapidstring.h"
#include <catch.hpp>
#include <string>

inline void validate_rapidstring(const rapidstring *s, const std::string &cmp)
{
	REQUIRE(rs_len(s) == cmp.length());
	REQUIRE(rs_data_c(s) == cmp);

	if (rs_is_heap(s))
		REQUIRE(rs_capacity(s) >= cmp.length());
	else
		REQUIRE(rs_capacity(s) == RS_STACK_CAPACITY);
}

#endif /* !UTILITY_HPP_ECB97D42D011D625 */
