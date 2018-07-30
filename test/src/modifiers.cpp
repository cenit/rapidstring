#include "utility.hpp"
#include <cstddef>
#include <cstring>

/* Theme: Game of Thrones. */

TEST_CASE("data")
{
	const std::string first{ "Winter" };
	const std::string second{
		"A mind needs books as a sword needs a whetstone, if it is to "
		"keep its edge."
	};

	rapidstring s1;
	rs_init_w(&s1, first.data());
	REQUIRE(first == rs_data(&s1));

	rapidstring s2;
	rs_init_w(&s2, second.data());
	REQUIRE(second == rs_data(&s2));

	rs_free(&s1);
	rs_free(&s2);
}

TEST_CASE("resize stack")
{
	constexpr std::size_t size{ 15 };
	std::string first{ "is" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	VALIDATE_RS(&s, first);

	rs_free(&s);
}

TEST_CASE("resize stack to heap")
{
	constexpr std::size_t size{ 100 };
	std::string first{ "coming" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	VALIDATE_RS(&s, first);

	rs_free(&s);
}

TEST_CASE("steal")
{
	constexpr std::size_t size{ 100 };

	const auto buffer = static_cast<char *>(RS_MALLOC(size));
	std::memset(buffer, 'a', size);

	rapidstring s;
	rs_steal(&s, buffer, size, size - 1);

	const auto sz = size - 1;
	REQUIRE(rs_cap(&s) == sz);
	REQUIRE(rs_len(&s) == sz);
	REQUIRE(rs_data_c(&s)[rs_len(&s)] == '\0');
	REQUIRE(std::strcmp(rs_data_c(&s), buffer) == 0);

	rs_free(&s);
}

template <typename F> void test_erase(std::string cmp, F functor)
{
	const std::string original{ cmp };

	rapidstring s;
	rs_init_w(&s, cmp.data());
	rs_erase(&s, 0, 0);
	VALIDATE_RS(&s, cmp);

	functor(&s, 0, 2);
	cmp.erase(0, 2);
	VALIDATE_RS(&s, cmp);

	cmp = original;
	rs_cpy(&s, cmp.data());
	functor(&s, 2, 2);
	cmp.erase(2, 2);
	VALIDATE_RS(&s, cmp);

	cmp = original;
	rs_cpy(&s, cmp.data());
	rs_erase(&s, 2, cmp.size() - 2);
	cmp.erase(2, cmp.size() - 2);
	VALIDATE_RS(&s, cmp);

	cmp = original;
	rs_cpy(&s, cmp.data());
	rs_erase(&s, 0, cmp.size());
	cmp.erase(0, cmp.size());
	VALIDATE_RS(&s, cmp);

	rs_free(&s);
}

TEST_CASE("erase")
{
	test_erase("Tyrion", rs_stack_erase);
	test_erase(
		"Power resides where men believe it resides. No more and no less.",
		rs_heap_erase);
}

TEST_CASE("clear")
{
	const std::string first{ "GOT" };
	const std::string second{
		"Love is poison. A sweet poison, yes, but it will kill you all the "
		"same."
	};
	const std::string empty;

	rapidstring s;
	rs_init_w(&s, first.data());
	rs_stack_clear(&s);
	VALIDATE_RS(&s, empty);

	rs_cpy(&s, first.data());
	rs_clear(&s);
	VALIDATE_RS(&s, empty);

	rs_cpy(&s, second.data());
	rs_heap_clear(&s);
	VALIDATE_RS(&s, empty);

	rs_cpy(&s, second.data());
	rs_clear(&s);
	VALIDATE_RS(&s, empty);

	rs_free(&s);
}

TEST_CASE("resize heap")
{
	constexpr std::size_t size{ 200 };
	std::string first{
		"Why is it that when one man builds a wall, the next man "
		"immediately needs to know what's on the other side?"
	};

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	VALIDATE_RS(&s, first);

	rs_free(&s);
}
