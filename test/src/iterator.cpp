#include "utility.hpp"
#include <cstddef>
#include <string>

TEST_CASE("Iterators")
{
	const std::string first{ "Short!" };
	const std::string second{ "A long string to get around SSO!" };

	rapid_string s1;
	rs_init_w(&s1, first.data());
	auto it1 = rs_begin(&s1);
	auto end1 = rs_end(&s1);

	for (std::size_t i = 0; it1 != end1; it1++, i++)
		REQUIRE(*it1 == first.at(i));

	rapid_string s2;
	rs_init_w(&s2, second.data());
	auto it2 = rs_begin(&s2);
	auto end2 = rs_end(&s2);

	for (std::size_t i = 0; it2 != end2; it2++, i++)
		REQUIRE(*it2 == second.at(i));
}
