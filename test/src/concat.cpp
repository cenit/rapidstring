#include "utility.hpp"

/* Theme: Stranger Things. */

TEST_CASE("Stack concatenation")
{
	const std::string first{ "Friends don't" };
	const std::string second{ " lie." };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	validate_rapidstring(&s, first);

	rs_cat(&s, second.data());

	validate_rapidstring(&s, first + second);

	rs_free(&s);
}

TEST_CASE("Stack and heap concatenation")
{
	const std::string first{ "Yeah, that's right! You better run!" };
	const std::string second{ "She's our friend and she's crazy!" };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	validate_rapidstring(&s, first);

	rs_cat(&s, second.data());

	validate_rapidstring(&s, first + second);

	rs_free(&s);
}

TEST_CASE("Heap concatenation")
{
	const std::string first{ "Our friend has superpowers, and she " };
	const std::string second{ "squeezed your tiny bladder with her mind." };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	validate_rapidstring(&s, first);

	rs_cat(&s, second.data());

	validate_rapidstring(&s, first + second);

	rs_free(&s);
}

TEST_CASE("String concatenation")
{
	const std::string first{ "If anyone asks where I am, " };
	const std::string second{ "I've left the country." };

	rapidstring s1;
	rs_init_w(&s1, first.data());

	rapidstring s2;
	rs_init_w(&s2, second.data());

	rs_cat_rs(&s1, &s2);

	validate_rapidstring(&s1, first + second);

	rs_free(&s1);
	rs_free(&s2);
}
