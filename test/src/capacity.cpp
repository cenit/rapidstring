#include "utility.hpp"
#include <cstddef>

/* Theme: Harry Potter. */

TEST_CASE("empty stack")
{
	const std::string first{ "Blimey!" };

	rapidstring s;
	rs_init(&s);

	REQUIRE(rs_empty(&s));

	rs_cpy(&s, first.data());

	REQUIRE(!rs_empty(&s));

	rs_free(&s);
}

TEST_CASE("empty heap")
{
	const std::string first{ "You're a wizard Harry... I'm a what?" };

	rapidstring s;
	rs_init(&s);

	REQUIRE(rs_empty(&s));

	rs_cpy(&s, first.data());

	REQUIRE(!rs_empty(&s));

	rs_free(&s);
}

TEST_CASE("reserve stack to heap")
{
	constexpr std::size_t cap{ 100 };
	const std::string first{ "Bloody hell!" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_reserve(&s, cap);

	REQUIRE(rs_cap(&s) >= cap);
	validate_rapidstring(&s, first);

	rs_free(&s);
}

TEST_CASE("reserve grow heap")
{
	constexpr std::size_t cap{ 300 };
	const std::string first{
		"THIRTY-ZERO! TAKE THAT, YOU DIRTY, CHEATING-"
		"Jordan, if you can't commentate in an unbaised way-!"
		"I'm telling it like it is, Professor!"
	};

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_reserve(&s, cap);

	REQUIRE(rs_cap(&s) >= cap);
	validate_rapidstring(&s, first);

	rs_free(&s);
}

TEST_CASE("shrink to fit stack")
{
	const std::string first{ "AVADA KEDAVRA!" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_shrink_to_fit(&s);

	validate_rapidstring(&s, first);

	rs_free(&s);
}

TEST_CASE("shrink to fit heap")
{
	const std::string first{
		"'Jiggery pokery!' said Harry in a fierce voice. 'Hocus "
		"pocus - squiggly wiggly -'"
		"'MUUUUUUM!' howled Dudley, 'He’s doing you know what!'"
	};

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_shrink_to_fit(&s);

	validate_rapidstring(&s, first);

	rs_free(&s);
}
