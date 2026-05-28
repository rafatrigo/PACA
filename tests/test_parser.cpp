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

    SECTION("Boolean flags can be retrieved via get<bool>") {
        parse_mock(parser, {"./app", "--verbose"});
        
        REQUIRE(parser.get<bool>("verbose") == true);
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

TEST_CASE("Parser extracts typed options correctly", "[parser][options]") {
    cli::Parser parser("PACA Options Test");

    // Register test options
    parser.add_option<std::string>('h', "host").default_value("localhost");
    parser.add_option<int>('p', "port").required();
    parser.add_option<double>("timeout"); // Long only
    parser.add_flag('v', "verbose", "Enable verbose logging");

    SECTION("Parse space-separated long options (--key value)") {
        parse_mock(parser, {"./app", "--host", "127.0.0.1", "--port", "8080", "--timeout", "2.5"});
        
        REQUIRE(parser.get<std::string>("host") == "127.0.0.1");
        REQUIRE(parser.get<int>("port") == 8080);
        REQUIRE(parser.get<double>("timeout") == 2.5);
    }

    SECTION("Parse equals-separated long options (--key=value)") {
        parse_mock(parser, {"./app", "--host=10.0.0.1", "--port=9090", "--timeout=5.0"});
        
        REQUIRE(parser.get<std::string>("host") == "10.0.0.1");
        REQUIRE(parser.get<int>("port") == 9090);
        REQUIRE(parser.get<double>("timeout") == 5.0);
    }

    SECTION("Parse space-separated short options (-k value)") {
        parse_mock(parser, {"./app", "-h", "0.0.0.0", "-p", "443"});
        
        REQUIRE(parser.get<std::string>("host") == "0.0.0.0");
        REQUIRE(parser.get<int>("port") == 443);
    }

    SECTION("Parse equals-separated short options (-k=value)") {
        parse_mock(parser, {"./app", "-h=192.168.1.1", "-p=80"});
        
        REQUIRE(parser.get<std::string>("host") == "192.168.1.1");
        REQUIRE(parser.get<int>("port") == 80);
    }

    SECTION("Fallback to default values when omitted") {
        parse_mock(parser, {"./app", "--port", "8080"});
        
        // Host was not passed, should return default "localhost"
        REQUIRE(parser.get<std::string>("host") == "localhost");
    }
}

TEST_CASE("Parser safely handles option errors and exceptions", "[parser][exceptions]") {
    cli::Parser parser("PACA Error Test");

    parser.add_option<int>('p', "port");
    parser.add_option<std::string>("name");

    SECTION("Throw ParseError on missing value for long option") {
        auto bad_parse = [&]() { parse_mock(parser, {"./app", "--port"}); };
        
        REQUIRE_THROWS_AS(bad_parse(), cli::ParseError);
        REQUIRE_THROWS_WITH(bad_parse(), Catch::Matchers::ContainsSubstring("Missing value"));
    }

    SECTION("Throw ParseError on missing value for short option") {
        auto bad_parse = [&]() { parse_mock(parser, {"./app", "-p"}); };
        
        REQUIRE_THROWS_AS(bad_parse(), cli::ParseError);
        REQUIRE_THROWS_WITH(bad_parse(), Catch::Matchers::ContainsSubstring("Missing value"));
    }

    SECTION("Throw ConversionError on invalid data type") {
        auto bad_parse = [&]() { parse_mock(parser, {"./app", "--port", "not-a-number"}); };
        
        REQUIRE_THROWS_AS(bad_parse(), cli::ConversionError);
    }

    SECTION("Throw ParseError (Type Mismatch) on bad get<T> request") {
        parse_mock(parser, {"./app", "--port", "8080"});
        
        // Developer error: registered as int, retrieving as double
        auto bad_get = [&]() { parser.get<double>("port"); };
        
        REQUIRE_THROWS_AS(bad_get(), cli::ParseError);
        REQUIRE_THROWS_WITH(bad_get(), Catch::Matchers::ContainsSubstring("Type mismatch"));
    }

    SECTION("Throw ParseError (Not Found) on unregistered option request") {
        parse_mock(parser, {"./app", "--port", "8080"});
        
        // Developer error: retrieving an option that was never added
        auto bad_get = [&]() { parser.get<std::string>("fake_option"); };
        
        REQUIRE_THROWS_AS(bad_get(), cli::ParseError);
        REQUIRE_THROWS_WITH(bad_get(), Catch::Matchers::ContainsSubstring("Option not found"));
    }
}
