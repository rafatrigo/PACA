#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "cli/detail/convert_impl.hpp"
#include "cli/exceptions.hpp"

TEST_CASE("String Conversion", "[convert]") {
    REQUIRE(cli::detail::convert_to<std::string>("hello world") == "hello world");
    REQUIRE(cli::detail::convert_to<std::string>("").empty());
}

TEST_CASE("Integer Conversion", "[convert][int]") {
    SECTION("Valid integers") {
        REQUIRE(cli::detail::convert_to<int>("42") == 42);
        REQUIRE(cli::detail::convert_to<int>("-100") == -100);
        REQUIRE(cli::detail::convert_to<int>("0") == 0);
    }

    SECTION("Invalid integers throw ConversionError") {
        REQUIRE_THROWS_AS(cli::detail::convert_to<int>("42abc"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<int>("abc"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<int>(""), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<int>("3.14"), cli::ConversionError);
    }
}

TEST_CASE("Floating Point Conversion", "[convert][float][double]") {
    SECTION("Valid floats and doubles") {
        REQUIRE_THAT(cli::detail::convert_to<double>("3.14"), Catch::Matchers::WithinRel(3.14));
        REQUIRE_THAT(cli::detail::convert_to<double>("-0.001"), Catch::Matchers::WithinRel(-0.001));
        REQUIRE_THAT(cli::detail::convert_to<double>("1.5e3"), Catch::Matchers::WithinRel(1500.0));
        
        REQUIRE_THAT(cli::detail::convert_to<float>("3.14"), Catch::Matchers::WithinRel(3.14f));
        REQUIRE_THAT(cli::detail::convert_to<float>("-2.5E-1"), Catch::Matchers::WithinRel(-0.25f));
    }

    SECTION("Invalid floats throw ConversionError") {
        REQUIRE_THROWS_AS(cli::detail::convert_to<double>("3.14abc"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<double>("abc"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<double>(""), cli::ConversionError);
        
        REQUIRE_THROWS_AS(cli::detail::convert_to<float>("3.14abc"), cli::ConversionError);
    }
}

TEST_CASE("Boolean Conversion", "[convert][bool]") {
    SECTION("Valid truthy values") {
        REQUIRE(cli::detail::convert_to<bool>("true") == true);
        REQUIRE(cli::detail::convert_to<bool>("TRUE") == true);
        REQUIRE(cli::detail::convert_to<bool>("TrUe") == true);
        REQUIRE(cli::detail::convert_to<bool>("1") == true);
        REQUIRE(cli::detail::convert_to<bool>("yes") == true);
        REQUIRE(cli::detail::convert_to<bool>("Y") == true);
        REQUIRE(cli::detail::convert_to<bool>("on") == true);
    }

    SECTION("Valid falsy values") {
        REQUIRE(cli::detail::convert_to<bool>("false") == false);
        REQUIRE(cli::detail::convert_to<bool>("FALSE") == false);
        REQUIRE(cli::detail::convert_to<bool>("0") == false);
        REQUIRE(cli::detail::convert_to<bool>("no") == false);
        REQUIRE(cli::detail::convert_to<bool>("N") == false);
        REQUIRE(cli::detail::convert_to<bool>("off") == false);
    }

    SECTION("Invalid booleans throw ConversionError") {
        REQUIRE_THROWS_AS(cli::detail::convert_to<bool>("maybe"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<bool>(""), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<bool>("2"), cli::ConversionError);
        REQUIRE_THROWS_AS(cli::detail::convert_to<bool>("true false"), cli::ConversionError);
    }
}

TEST_CASE("Exception Payload Validation", "[convert][exceptions]") {
    SECTION("Integer exception payload") {
        try {
            cli::detail::convert_to<int>("bad_int");
            FAIL("Expected ConversionError");
        } catch (const cli::ConversionError& e) {
            REQUIRE(e.value() == "bad_int");
            REQUIRE(e.target_type() == "int");
        }
    }

    SECTION("Double exception payload") {
        try {
            cli::detail::convert_to<double>("bad_double");
            FAIL("Expected ConversionError");
        } catch (const cli::ConversionError& e) {
            REQUIRE(e.value() == "bad_double");
            REQUIRE(e.target_type() == "double");
        }
    }
}
