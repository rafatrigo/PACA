#include <iostream>
#include <string>
#include "cli/cli.hpp"

int main(int argc, char** argv) {
    // initialize the parser with a description
    cli::Parser app("PACA Help Menu Demonstration App");

    // register some options and flags to populate the help menu
    app.add_option<std::string>('n', "name")
       .description("The name of the user")
       .required();

    app.add_option<int>('p', "port")
       .description("Port to bind the server to")
       .default_value(8080);

    app.add_option<std::string>("config")
       .description("Path to the configuration file"); // Long-only option

    app.add_flag('v', "verbose", "Enable verbose logging");

    try {
        // parse the arguments. 
        // if the user passes -h or --help, PACA will print the help menu and exit here!
        app.parse(argc, argv);

        // standard application logic (won't run if --help is passed)
        std::cout << "Parsing successful!\n";
        std::cout << "Hello, " << app.get<std::string>("name") << "!\n";
        
        if (app.is_set("verbose")) {
            std::cout << "Verbose mode is active. Server starting on port " 
                      << app.get<int>("port") << "...\n";
        }

    } catch (const cli::ParseError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Run with --help for usage information.\n";
        return 1;
    }

    return 0;
}
