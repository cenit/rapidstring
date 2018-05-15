#include "utility.hpp"
#include <string>

// TODO: stack assignment with stack_assign and assign, same for heap.

TEST_CASE("Stack assignment")
{
	const std::string first{ "Very " };
	const std::string second{ "short!" };

	rapid_string s;
	rs_init(&s);
	rs_stack_assign(&s, first.data());

	CMP_STR(&s, first);

	rs_stack_assign(&s, second.data());

	CMP_STR(&s, second);

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

	CMP_STR(&s, first);

	rs_reserve(&s, second.length());
	rs_heap_assign(&s, second.data());

	CMP_STR(&s, second);

	rs_free(&s);
}

TEST_CASE("Stack and heap assignment")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapid_string s;
	rs_init(&s);
	rs_assign(&s, first.data());

	CMP_STR(&s, first);

	rs_assign(&s, second.data());
	
	CMP_STR(&s, second);

	rs_free(&s);
}
