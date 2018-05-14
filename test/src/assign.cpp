#include "utility.hpp"
#include <string>

TEST_CASE("Stack assignment")
{
	const std::string first{ "Very " };
	const std::string second{ "short!" };

	rapid_string s;
	rs_init(&s);
	rs_stack_assign(&s, first.data());
	rs_stack_assign(&s, second.data());

	CMP_STR(&s, first + second);

	rs_free(&s);
}

TEST_CASE("Heap assignment")
{
	const std::string first{ "A very long string to get around SSO!" };
	const std::string second{ "Today is a fine day. Let's go." };

	rapid_string s;

	// Explicit capacity init required as heap_assign doesn't allocate.
	rs_init_w_cap(&s, first.length());
	rs_heap_assign(&s, first.data());

	rs_reserve(&s, rs_size(&s) + second.length());
	rs_heap_assign(&s, second.data());

	CMP_STR(&s, first);

	rs_free(&s);
}

TEST_CASE("Stack and heap assignment")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapid_string s1;
	rs_init(&s1);
	rs_assign(&s1, first.data());

	CMP_STR(&s1, first);

	rapid_string s2;
	rs_init(&s2);
	rs_assign(&s2, second.data());
	
	CMP_STR(&s2, second);

	rs_free(&s1);
	rs_free(&s2);
}
