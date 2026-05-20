#include <catch2/catch_test_macros.hpp> 
#include <cli/exceptions.hpp>
#include <string>

TEST_CASE("CLI Exceptions behave correctly", "[exceptions]") {
    
    SECTION("UnknownArgument stores and formats the bad argument name") {
        // 1. Manually throw the exception inside a try block
        try {
            throw cli::UnknownArgument("--invalid-flag");
        } 
        // 2. Catch it by reference
        catch (const cli::UnknownArgument& e) {
            // 3. Verify the error message is exactly what you expect
            std::string expected_message = "Unknown argument: --invalid-flag";
            REQUIRE(std::string(e.what()) == expected_message);
            
            // 4. Verify programmatic access to the raw argument
            REQUIRE(e.argument_name() == "--invalid-flag");
        }
    }

    SECTION("ConversionError stores and formats the bad value and target type") {
        try {
            throw cli::ConversionError("abc", "integer");
        } 
        catch (const cli::ConversionError& e) {
            std::string expected_message = "Failed to convert 'abc' to integer";
            REQUIRE(std::string(e.what()) == expected_message);
            
            REQUIRE(e.value() == "abc");
            REQUIRE(e.target_type() == "integer");
        }
    }

    SECTION("ParseError acts as a base class") {
        // Verify that UnknownArgument can be caught as a generic ParseError
        try {
            throw cli::UnknownArgument("-x");
        } catch (const cli::ParseError& e) {
            REQUIRE(std::string(e.what()) == "Unknown argument: -x");
        }

        // Verify that ConversionError can be caught as a generic ParseError
        try {
            throw cli::ConversionError("3.14", "boolean");
        } catch (const cli::ParseError& e) {
            REQUIRE(std::string(e.what()) == "Failed to convert '3.14' to boolean");
        }
    }
}
