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
          m_argument_name(argument_name) {}

    /**
     * @brief Retrieves the name of the unknown argument.
     * @return The raw argument string that caused the failure.
     */
    const std::string& argument_name() const noexcept {
        return m_argument_name;
    }

private:
    std::string m_argument_name;
};

} // namespace cli
