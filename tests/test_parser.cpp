#include <catch2/catch_test_macros.hpp>
#include <cli/cli.hpp>

TEST_CASE("Library version is accessible", "[core]") {
    REQUIRE(std::string(cli::version()) == "0.1.0");
}
