#include "utility.hpp"
#include <string>

TEST_CASE("data")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapidstring s1;
	rs_init_w(&s1, first.data());
	REQUIRE(first == rs_data(&s1));

	rapidstring s2;
	rs_init_w(&s2, second.data());
	REQUIRE(second == rs_data(&s2));
	
	rs_free(&s1);
	rs_free(&s2);
}

