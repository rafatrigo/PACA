#pragma once

#include <stdexcept>
#include <string>

namespace cli {

/**
 * @brief Base class for all parsing-related errors in PACA.
 */
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when an unrecognized argument is encountered.
 */
class UnknownArgument : public ParseError {
public:
    explicit UnknownArgument(const std::string& argument_name)
        : ParseError("Unknown argument: " + argument_name),
          argument_name_(argument_name) {}

    /**
     * @brief Retrieves the name of the unknown argument.
     * @return The raw argument string that caused the failure.
     */
    const std::string& argument_name() const noexcept {
        return argument_name_;
    }

private:
    std::string argument_name_;
};

} // namespace cli
