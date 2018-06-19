#include "utility.hpp"
#include <cstddef>
#include <cstring>

/* Theme: Game of Thrones. */

void validate_steal(const rapidstring *s, const char *buffer, std::size_t size)
{
	REQUIRE(rs_capacity(s) == size);
	REQUIRE(rs_len(s) == size);
	REQUIRE(rs_data_c(s)[rs_len(s)] == '\0');
	REQUIRE(std::strcmp(rs_data_c(s), buffer) == 0);
}

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
	constexpr std::size_t size{ 100 };

	auto buffer = static_cast<char *>(RS_MALLOC(size));
	std::memset(buffer, 'a', size);

	rapidstring s;
	rs_init(&s);
	rs_steal(&s, buffer, size);

	validate_steal(&s, buffer, size - 1);

	rs_free(&s);
}

TEST_CASE("steal with capacity")
{
	constexpr std::size_t size{ 100 };

	/* Filler data to be freed before the buffer is stolen. */
	const std::string first{
		"The things we love destroy us every time, lad. Remember that."
	};

	auto buffer = static_cast<char *>(RS_MALLOC(size));
	std::memset(buffer, 'a', size);

	rapidstring s;
	rs_init_w(&s, first.data());
	rs_steal(&s, buffer, size);

	validate_steal(&s, buffer, size - 1);

	rs_free(&s);
}

TEST_CASE("resize stack")
{
	constexpr std::size_t size{ 15 };
	std::string first{ "is" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	validate_rapidstring(&s, first);

	rs_free(&s);
}

TEST_CASE("resize stack to heap")
{
	constexpr std::size_t size{ 100 };
	std::string first{ "is" };

	rapidstring s;
	rs_init_w(&s, first.data());

	rs_resize_w(&s, size, 'a');
	first.resize(size, 'a');

	validate_rapidstring(&s, first);

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

	validate_rapidstring(&s, first);

	rs_free(&s);
}
