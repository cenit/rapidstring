#include "utility.hpp"
#include <string>

// Theme: Stranger Things.

TEST_CASE("Unsafe stack concatenation")
{
	const std::string first{ "El's " };
	const std::string second{ " bitchin'." };
	const std::string sum{ first + second };

	rapidstring s;
	rs_init(&s);

	rs_stack_cat(&s, first.data());

	CMP_STR(&s, first);

	rs_stack_cat(&s, second.data());

	CMP_STR(&s, sum);

	rs_free(&s);
}

TEST_CASE("Stack concatenation")
{
	const std::string first{ "Friends don't" };
	const std::string second{ " lie." };
	const std::string sum{ first + second };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	CMP_STR(&s, first);

	rs_cat(&s, second.data());

	CMP_STR(&s, sum);

	rs_free(&s);
}

TEST_CASE("Stack and heap concatenation")
{
	const std::string first{ "Yeah, that's right! You better run!" };
	const std::string second{ "She's our friend and she's crazy!" };
	const std::string sum{ first + second };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	CMP_STR(&s, first);

	rs_cat(&s, second.data());

	CMP_STR(&s, sum);

	rs_free(&s);
}

TEST_CASE("Unsafe heap concatenation")
{
	const std::string first{
		"You always say we should never stop being curious, "
		"to always open any curiosity door we find. "
	};
	const std::string second{
		"Why are you keeping this curiosity door locked?"
	};
	const std::string sum{ first + second };

	rapidstring s;
	rs_init_w_cap(&s, sum.size());

	rs_heap_cat(&s, first.data());

	CMP_STR(&s, first);

	rs_heap_cat(&s, second.data());

	CMP_STR(&s, sum);

	rs_free(&s);
}

TEST_CASE("Heap concatenation")
{
	const std::string first{
		"Our friend has superpowers, and she "
	};
	const std::string second{
		"squeezed your tiny bladder with her mind."
	};
	const std::string sum{ first + second };

	rapidstring s;
	rs_init(&s);

	rs_cat(&s, first.data());

	CMP_STR(&s, first);

	rs_cat(&s, second.data());

	CMP_STR(&s, sum);

	rs_free(&s);
}

TEST_CASE("String concatenation")
{
	const std::string first{ "If anyone asks where I am, " };
	const std::string second{ "I've left the country." };
	const std::string sum{ first + second };

	rapidstring s1;
	rs_init_w(&s1, first.data());

	rapidstring s2;
	rs_init_w(&s2, second.data());

	rs_cat_rs(&s1, &s2);

	CMP_STR(&s1, sum);

	rs_free(&s1);
	rs_free(&s2);
}
