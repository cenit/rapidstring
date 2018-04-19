#pragma once

#include "rapid_string.h"
#include <catch.hpp>

#define CMP_STR(s, cmp)					   \
	REQUIRE(rs_len(s) == cmp.length());		   \
	REQUIRE(rs_buff(s) == cmp);			   \
							   \
	if (rs_is_heap(s))				   \
		REQUIRE(rs_cap(s) >= cmp.length());	   \
	else						   \
		REQUIRE(rs_cap(s) == RS_STACK_CAPACITY)

#define CMP_STR_RC(rc, s, cmp) \
	REQUIRE(!rc);	       \
	CMP_STR(s, cmp)
