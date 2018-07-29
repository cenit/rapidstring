#include "utility.hpp"

TEST_CASE("Stack and heap copying")
{
	const std::string first{ "Short!" };
	const std::string second{ "A very long string to get around SSO!" };

	rapidstring s;
	rs_init(&s);
	rs_cpy(&s, first.data());

	VALIDATE_RS(&s, first);

	rs_cpy(&s, second.data());

	VALIDATE_RS(&s, second);

	rs_free(&s);
}

TEST_CASE("Heap growth copying")
{
	const std::string first{ "A very long string to get around SSO!" };

	rapidstring s;
	rs_init_w_cap(&s, 1);

	rs_cpy(&s, first.data());

	VALIDATE_RS(&s, first);

	rs_free(&s);
}

TEST_CASE("String copying")
{
	const std::string first{ "Visual Basic is the best language." };

	rapidstring s1;
	rs_init(&s1);

	rapidstring s2;
	rs_init_w(&s2, first.data());

	rs_cpy_rs(&s1, &s2);

	VALIDATE_RS(&s1, first);

	rs_free(&s1);
	rs_free(&s2);
}
