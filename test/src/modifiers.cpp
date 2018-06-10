#include "utility.hpp"
#include <cstring>
#include <string>

// Theme: Game of Thrones.

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

TEST_CASE("steal")
{
	const std::string first{
		"When the snows fall and the white winds blow, the lone wolf "
		"dies but the pack survives."
	};

	rapidstring s1;
	rs_init_w(&s1, first.data());

	rapidstring s2;
	rs_init(&s2);

	rs_steal(&s2, rs_data(&s1));

	CMP_STR(&s2, first);

	/*
	 * Only the second string is freed as it stole the first string's
	 * buffer.
	 */
	rs_free(&s2);
}

TEST_CASE("steal with capacity")
{
	constexpr std::uint8_t size{ 100 };
	constexpr std::uint8_t usable_size{ size - 1 };

	// Filler data to be freed before the buffer is stolen.
	const std::string first{
		"The things we love destroy us every time, lad. Remember that."
	};

	auto buffer = static_cast<char *>(RS_MALLOC(size));
	std::memset(buffer, 'a', size);

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_steal_n(&s, buffer, size);

	REQUIRE(rs_capacity(&s) == usable_size);
	REQUIRE(rs_len(&s) == usable_size);
	REQUIRE(rs_data_c(&s)[rs_len(&s)] == '\0');
	REQUIRE(std::strcmp(rs_data_c(&s), buffer) == 0);

	rs_free(&s);
}

TEST_CASE("resize stack")
{
	constexpr std::uint8_t size{ 15 };
	std::string first{ "is" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	CMP_STR(&s, first);

	rs_free(&s);
}

TEST_CASE("resize stack to heap")
{
	constexpr std::uint8_t size{ 100 };
	std::string first{ "is" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	CMP_STR(&s, first);

	rs_free(&s);
}

TEST_CASE("resize heap")
{
	constexpr std::uint8_t size{ 200 };
	std::string first{
		"Why is it that when one man builds a wall, the next man "
		"immediately needs to know what's on the other side?"
	};

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	CMP_STR(&s, first);

	rs_free(&s);
}
