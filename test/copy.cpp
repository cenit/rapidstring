#include "utility.hpp"
#include <string>

TEST_CASE("Stack copying")
{
	const std::string first{ "Hello World!" };
	const std::string second{ "Short!" };

	auto s{ rs_empty() };
	auto rc = rs_cpy(&s, first.c_str());

	CMP_STR_RC(rc, &s, first);

	rc = rs_cpy(&s, second.c_str());

	CMP_STR_RC(rc, &s, second);

	rs_free(&s);
}

TEST_CASE("Stack and heap copying")
{
	const std::string first{ "Hello World!" };
	const std::string second{ "A long string to get around SSO!" };

	auto s{ rs_empty() };
	auto rc = rs_cpy(&s, first.c_str());

	CMP_STR_RC(rc, &s, first);

	rc = rs_cpy(&s, second.c_str());

	CMP_STR_RC(rc, &s, second);

	rs_free(&s);
}

TEST_CASE("Heap copying")
{
	const std::string first{ "A long string to get around SSO!" };
	const std::string second{ "This is not a short string." };

	auto s{ rs_empty() };
	auto rc = rs_cpy(&s, first.c_str());

	CMP_STR_RC(rc, &s, first);

	rc = rs_cpy(&s, second.c_str());

	CMP_STR_RC(rc, &s, second);

	rs_free(&s);
}
