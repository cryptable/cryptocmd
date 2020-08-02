#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "OpenSSLInit.h"

int main(int argc, char* argv[]) {
	// global setup...
    OpenSSLInit openSslInit;

	int result = Catch::Session().run(argc, argv);

	// global clean-up...

	return result;
}