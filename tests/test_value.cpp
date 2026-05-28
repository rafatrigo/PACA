#include <catch2/catch_test_macros.hpp>
#include <string>

#include "cli/value.hpp"
#include "cli/exceptions.hpp"

TEST_CASE("Value prioritization and fallbacks", "[value]") {
    // Initialize with a mock option name for error reporting testing
    cli::Value<int> val("--port");

    SECTION("Initial state is empty and throws on access") {
        REQUIRE(val.has_value() == false);
        
        // Ensure it throws the exact exception type we designed
        REQUIRE_THROWS_AS(val.get(), cli::ValueNotSet);
    }

    SECTION("Exception carries the correct option name") {
        try {
            // Explicitly cast to void to silence the [[nodiscard]] warning
            (void)val.get();
            FAIL("Expected ValueNotSet exception to be thrown");
        } catch (const cli::ValueNotSet& e) {
            // Verify our programmatic getter works
            REQUIRE(e.option_name() == "--port");
            
            // Verify the human-readable message is correctly formatted
            std::string expected_msg = "Value requested but was not set and has no default: --port";
            REQUIRE(std::string(e.what()) == expected_msg);
        }
    }

    SECTION("Default value is returned if no explicit value exists") {
        val.set_default(8080);
        
        REQUIRE(val.has_value() == true);
        REQUIRE(val.get() == 8080);
    }

    SECTION("Explicitly set value is returned when no default exists") {
        val.set(9000);
        
        REQUIRE(val.has_value() == true);
        REQUIRE(val.get() == 9000);
    }

    SECTION("Parsed value correctly overrides the default value") {
        val.set_default(8080);
        val.set(9000); // User provided --port 9000 on the command line
        
        REQUIRE(val.has_value() == true);
        REQUIRE(val.get() == 9000); // Parsed value must win
    }
}

TEST_CASE("Value handles non-trivial types (std::string)", "[value][string]") {
    cli::Value<std::string> str_val("--host");

    SECTION("String values fallback correctly") {
        str_val.set_default("localhost");
        REQUIRE(str_val.get() == "localhost");
    }

    SECTION("String values override correctly") {
        str_val.set_default("localhost");
        str_val.set("127.0.0.1");
        
        REQUIRE(str_val.get() == "127.0.0.1");
    }
}
