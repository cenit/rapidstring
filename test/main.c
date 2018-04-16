#include "rapid_str.h"
#include <assert.h>
#include <string.h>

#define SHORT_STR "Short"
#define CAT " string"
#define LONG_STR "Long enough to get around SSO"

int main(void)
{
	int rc;
	rapid_str short_str;
	rapid_str_init(&short_str);

	assert(!strcmp(rapid_str_buff(&short_str), ""));
	assert(rapid_str_size(&short_str) == 0);
	assert(rapid_str_capacity(&short_str) == sizeof(short_str.stack));

	rc = rapid_str_copy(&short_str, SHORT_STR);
	
	assert(!rc);
	assert(!strcmp(rapid_str_buff(&short_str), SHORT_STR));
	assert(rapid_str_size(&short_str) == sizeof(SHORT_STR) - 1);

	rc = rapid_str_cat(&short_str, CAT);

	assert(!rc);
	assert(!strcmp(rapid_str_buff(&short_str), SHORT_STR CAT));
	assert(rapid_str_size(&short_str) == sizeof(SHORT_STR CAT) - 1);

	rapid_str long_str;
	rapid_str_init(&long_str);
	rc = rapid_str_copy(&long_str, LONG_STR);

	assert(!rc);
	assert(!strcmp(rapid_str_buff(&long_str), LONG_STR));
	assert(rapid_str_size(&long_str) == sizeof(LONG_STR) - 1);

	rc = rapid_str_cat(&long_str, CAT);

	assert(!rc);
	assert(!strcmp(rapid_str_buff(&long_str), LONG_STR CAT));
	assert(rapid_str_size(&long_str) == sizeof(LONG_STR CAT) - 1);

	rc = rapid_str_shrink_to_fit(&long_str);

	assert(!rc);
	assert(rapid_str_size(&long_str) == rapid_str_capacity(&long_str));
	assert(!strcmp(rapid_str_buff(&long_str), LONG_STR CAT));
	assert(rapid_str_size(&long_str) == sizeof(LONG_STR CAT) - 1);

	return 0;
}
