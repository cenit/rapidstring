#include "utility.hpp"
#include <string>

TEST_CASE("Stack copying")
{
	const std::string first{ "Hello World!" };
	const std::string second{ "Short!" };

	rapid_string s;
	rs_init(&s);

	rs_assign(&s, first.c_str());

	CMP_STR(&s, first);

	rs_assign(&s, second.c_str());

	CMP_STR(&s, second);

	rs_free(&s);
}

TEST_CASE("Stack and heap copying")
{
	const std::string first{ "Hello World!" };
	const std::string second{ "A long string to get around SSO!" };

	rapid_string s;
	rs_init(&s);

	rs_assign(&s, first.c_str());

	CMP_STR(&s, first);

	rs_assign(&s, second.c_str());

	CMP_STR(&s, second);

	rs_free(&s);
}

TEST_CASE("Heap copying")
{
	const std::string first{ "A long string to get around SSO!" };
	const std::string second{ "This is not a short string." };

	rapid_string s;
	rs_init(&s);
	
	rs_assign(&s, first.c_str());

	CMP_STR(&s, first);

	rs_assign(&s, second.c_str());

	CMP_STR(&s, second);

	rs_free(&s);
}
