#include <catch2/catch_test_macros.hpp>
#include <string>

#include "cli/option.hpp"
#include "cli/exceptions.hpp"

TEST_CASE("Option construction and default attributes", "[option]") {
    SECTION("Construct with long name only") {
        cli::Option<int> opt("port");
        
        REQUIRE(opt.long_name() == "port");
        REQUIRE(opt.short_name() == '\0');
        REQUIRE(opt.is_required() == false);
        REQUIRE(opt.description() == "");
    }

    SECTION("Construct with short and long name") {
        cli::Option<std::string> opt('h', "host");
        
        REQUIRE(opt.short_name() == 'h');
        REQUIRE(opt.long_name() == "host");
    }
}

TEST_CASE("Option fluent builder pattern", "[option]") {
    cli::Option<int> opt("port");

    // Chain the builder methods
    opt.description("The port to bind the server to")
       .required()
       .default_value(8080);

    // Verify all internal state was updated correctly
    REQUIRE(opt.description() == "The port to bind the server to");
    REQUIRE(opt.is_required() == true);
    
    // Verify the underlying Value container received the default
    REQUIRE(opt.value().has_value() == true);
    REQUIRE(opt.value().get() == 8080);
}

TEST_CASE("OptionBase type-erased parsing", "[option][polymorphism]") {
    cli::Option<double> opt('t', "timeout");
    
    // Create a base class reference to simulate what the Parser does
    cli::OptionBase& base_ref = opt;

    SECTION("Valid string parsing routes to derived class") {
        base_ref.parse_and_set("3.14");
        
        REQUIRE(opt.value().has_value() == true);
        REQUIRE(opt.value().get() == 3.14);
    }

    SECTION("Invalid string parsing throws ConversionError") {
        // Since T is double, "not_a_number" should fail inside convert_to<double>
        REQUIRE_THROWS_AS(base_ref.parse_and_set("not_a_number"), cli::ConversionError);
    }
}

TEST_CASE("Option correctly overrides default value on parse", "[option]") {
    cli::Option<int> opt("threads");
    
    opt.default_value(4);
    
    // Simulate user passing --threads 16
    opt.parse_and_set("16");
    
    // The parsed value should override the default
    REQUIRE(opt.value().has_value() == true);
    REQUIRE(opt.value().get() == 16);
}
