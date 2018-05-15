#include "utility.hpp"
#include <string>

TEST_CASE("at")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapidstring s1;
	rs_init_w(&s1, first.data());
	REQUIRE(rs_at(&s1, 0) == first.at(0));
	REQUIRE(rs_at(&s1, 3) == first.at(3));
	REQUIRE(rs_at(&s1, rs_size(&s1) - 1) == first.back());

	rapidstring s2;
	rs_init_w(&s2, second.data());
	REQUIRE(rs_at(&s2, 0) == second.at(0));
	REQUIRE(rs_at(&s2, 3) == second.at(3));
	REQUIRE(rs_at(&s2, rs_size(&s2) - 1) == second.back());

	rs_free(&s1);
	rs_free(&s2);
}

TEST_CASE("front")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapidstring s1;
	rs_init_w(&s1, first.data());
	REQUIRE(rs_front(&s1) == first.front());

	rapidstring s2;
	rs_init_w(&s2, second.data());
	REQUIRE(rs_front(&s2) == second.front());

	rs_free(&s1);
	rs_free(&s2);
}

TEST_CASE("back")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapidstring s1;
	rs_init_w(&s1, first.data());
	REQUIRE(rs_back(&s1) == first.back());

	rapidstring s2;
	rs_init_w(&s2, second.data());
	REQUIRE(rs_back(&s2) == second.back());

	rs_free(&s1);
	rs_free(&s2);
}

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

