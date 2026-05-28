#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include "exceptions.hpp"
#include "option.hpp"

namespace cli {

/**
 * @brief The core command-line argument parser.
 * 
 * The Parser class is responsible for registering expected command-line 
 * options and safely parsing `argc` and `argv` to extract their values.
 */
class Parser {
public:

    /**
     * @brief Constructs a new Parser.
     * @param app_description A brief description of the application, used in help output.
     */
    explicit Parser(std::string app_description = "");

    /**
     * @brief Registers a boolean flag (Query-based).
     * 
     * @param short_name The single-character short flag (e.g., 'v' for -v).
     * @param long_name The full word long flag (e.g., "verbose" for --verbose).
     * @param desc A description of the flag for the help menu.
     */
    void add_flag(char short_name, std::string_view long_name, std::string_view desc);

    /**
     * @brief Registers a boolean flag and binds it to a caller-owned variable (Zero-overhead).
     * 
     * The bound variable will be initialized to `false` upon registration, 
     * and set to `true` if the flag is encountered during parsing.
     * 
     * @param short_name The single-character short flag (e.g., 'v').
     * @param long_name The full word long flag (e.g., "verbose").
     * @param desc A description of the flag for the help menu.
     * @param bound_var Reference to the user's boolean variable.
     */
    void add_flag(char short_name, std::string_view long_name, std::string_view desc, bool& bound_var);

    /**
     * @brief Registers an option with a specific type (defaults to std::string).
     *
     * @param long_name The full word long option (e.g., "port").
     */
    template <typename T = std::string>
    Option<T>& add_option(std::string_view long_name);

    /**
     * @brief Registers an option with a specific type (defaults to std::string).
     *
     * @param short_name The single-character short option (e.g., 'p').
     * @param long_name The full word long option (e.g., "port").
     */
    template <typename T = std::string>
    Option<T>& add_option(char short_name, std::string_view long_name);

    /**
     * @brief Retrieves the typed value of an option or flag.
     * @throws cli::ParseError if the type requested mismatches the registered type.
     */
    template <typename T>
    T get(std::string_view long_name) const;

    /**
     * @brief Parses the command-line arguments.
     * 
     * @param argc The number of arguments passed by the OS.
     * @param argv The array of argument strings passed by the OS.
     * @throws cli::UnknownArgument If an unregistered flag is encountered.
     */
    void parse(int argc, const char* const* argv);

    /**
     * @brief Checks if a specific flag was provided during parsing.
     * 
     * @param long_name The long name of the registered flag to check.
     * @return true if the flag was parsed, false otherwise.
     */
    [[nodiscard]] bool is_set(std::string_view long_name) const;

    /**
     * @brief Generates a formatted help menu string based on registered options and flags.
     * @return A formatted std::string containing usage and descriptions.
     */
    [[nodiscard]] std::string generate_help() const;

private:
    struct Flag {
        char short_name;
        std::string long_name;
        std::string description;
        bool internal_state;
        bool* bound_value;
    };

    std::string description_;
    std::vector<Flag> flags_;

    std::vector<std::unique_ptr<OptionBase>> options_;

    // Internal helpers declared here, but defined in impl
    void set_flag_state(Flag& flag);
    void handle_long_flag(std::string_view name, std::string_view original_arg);
    void handle_short_flag(char name, std::string_view original_arg);

    bool handle_long_option(std::string_view name, std::string_view value);
    bool handle_short_option(char name, std::string_view value);
    bool is_long_option(std::string_view name) const;
    bool is_short_option(char name) const;
};

} // namespace cli

// This allows the impl file to see the Parser class definition.
#include "detail/parser_impl.hpp"
