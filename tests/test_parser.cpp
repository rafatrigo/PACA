#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <vector>
#include <string>


#include "cli/parser.hpp"

/*
 *By using a lambda we avoid to do something like this:
    const char* fake_argv[] = {"./app", "--verbose", "-d"};
    int fake_argc = sizeof(fake_argv) / sizeof(fake_argv[0]);
    parser.parse(fake_argc, fake_argv);
 * */

// Helper lambda to easily mock argc and argv
auto parse_mock = [](cli::Parser& parser, std::vector<const char*> args) {
    // args.size() implicitly converts to int, args.data() is const char* const*
    parser.parse(static_cast<int>(args.size()), args.data());
};

TEST_CASE("Parser core flag logic", "[parser][flags]") {
    // Shared Setup: This runs fresh for every SECTION below
    cli::Parser parser("PACA Test Suite");
    
    // Register query-based flags
    parser.add_flag('v', "verbose", "Enable verbose output");
    
    // Register bound flags
    bool debug_mode = true; // Initialize to true to verify it gets reset to false
    parser.add_flag('d', "debug", "Enable debug mode", debug_mode);

    SECTION("Flags are false by default") {
        parse_mock(parser, {"./app"}); // No arguments passed
        
        REQUIRE(parser.is_set("verbose") == false);
        REQUIRE(parser.is_set("debug") == false);
        REQUIRE(debug_mode == false); // Should be reset by add_flag
    }

    SECTION("Parse short flags successfully") {
        parse_mock(parser, {"./app", "-v", "-d"});
        
        REQUIRE(parser.is_set("verbose") == true);
        REQUIRE(parser.is_set("debug") == true);
        REQUIRE(debug_mode == true);
    }

    SECTION("Parse long flags successfully") {
        parse_mock(parser, {"./app", "--verbose", "--debug"});
        
        REQUIRE(parser.is_set("verbose") == true);
        REQUIRE(parser.is_set("debug") == true);
        REQUIRE(debug_mode == true);
    }

    SECTION("Throw UnknownArgument on unregistered long flag") {
        auto bad_parse = [&]() { parse_mock(parser, {"./app", "--unknown"}); };
        
        // Check that it throws the correct exception type
        REQUIRE_THROWS_AS(bad_parse(), cli::UnknownArgument);
        
        // Check that the exception message contains the exact invalid argument
        REQUIRE_THROWS_WITH(bad_parse(), Catch::Matchers::ContainsSubstring("--unknown"));
    }

    SECTION("Throw UnknownArgument on unregistered short flag") {
        auto bad_parse = [&]() { parse_mock(parser, {"./app", "-x"}); };
        
        REQUIRE_THROWS_AS(bad_parse(), cli::UnknownArgument);
        REQUIRE_THROWS_WITH(bad_parse(), Catch::Matchers::ContainsSubstring("-x"));
    }

    SECTION("Safely ignore standalone hyphens") {
        // Maybe it can throw a error in the future
        REQUIRE_NOTHROW(parse_mock(parser, {"./app", "-", "--"}));
        
        REQUIRE(parser.is_set("verbose") == false);
        REQUIRE(debug_mode == false);
    }
}
