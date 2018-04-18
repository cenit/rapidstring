#include "rapid_string.h"
#include <assert.h>
#include <string.h>

// TODO: benchmarks with clocking!

#define SHORT_STR "Short"
#define CAT " string"
#define LONG_CAT " is now a long string"
#define LONG_STR "Long enough to get around SSO"

int main(void)
{
	int rc;
	rapid_string short_str = rs_empty();

	assert(!strcmp(rs_buff(&short_str), ""));
	assert(rs_len(&short_str) == 0);
	assert(rs_cap(&short_str) == sizeof(short_str.stack.buff));

	rc = rs_cpy(&short_str, SHORT_STR);
	
	assert(!rc);
	assert(!strcmp(rs_buff(&short_str), SHORT_STR));
	assert(rs_len(&short_str) == sizeof(SHORT_STR) - 1);

	rc = rs_cat(&short_str, CAT);

	assert(!rc);
	assert(!strcmp(rs_buff(&short_str), SHORT_STR CAT));
	assert(rs_len(&short_str) == sizeof(SHORT_STR CAT) - 1);

	rc = rs_cat(&short_str, LONG_CAT);

	assert(!rc);
	assert(!strcmp(rs_buff(&short_str), SHORT_STR CAT LONG_CAT));
	assert(rs_len(&short_str) == sizeof(SHORT_STR CAT LONG_CAT) - 1);

	rapid_string long_str;
	rc = rs_init_w(&long_str, LONG_STR);

	assert(!rc);
	assert(!strcmp(rs_buff(&long_str), LONG_STR));
	assert(rs_len(&long_str) == sizeof(LONG_STR) - 1);

	rc = rs_cat(&long_str, CAT);

	assert(!rc);
	assert(!strcmp(rs_buff(&long_str), LONG_STR CAT));
	assert(rs_len(&long_str) == sizeof(LONG_STR CAT) - 1);

	rc = rs_shrink_to_fit(&long_str);

	assert(!rc);
	assert(rs_len(&long_str) == rs_cap(&long_str));
	assert(!strcmp(rs_buff(&long_str), LONG_STR CAT));
	assert(rs_len(&long_str) == sizeof(LONG_STR CAT) - 1);

	return 0;
}
