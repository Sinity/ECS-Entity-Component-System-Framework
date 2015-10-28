#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include "../src/utils/config.h"

int main(int argc, char* const argv[]) {
    auto retval = Catch::Session().run(argc, argv);
    return retval;
}
