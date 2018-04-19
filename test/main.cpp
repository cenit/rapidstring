#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <rpmalloc.h>

int main() {
	auto rc = rpmalloc_initialize();

	if (rc)
		return rc;

	rc = Catch::Session().run();

	rpmalloc_finalize();

	return rc;
}
