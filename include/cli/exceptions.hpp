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

/**
 * @brief Exception thrown when string-to-type conversion fails.
 */
class ConversionError : public ParseError {
public:
    ConversionError(const std::string& value, const std::string& target_type)
        : ParseError("Failed to convert '" + value + "' to " + target_type),
          value_(value),
          target_type_(target_type) {}

    /**
     * @brief Retrieves the raw string value that failed to convert.
     */
    const std::string& value() const noexcept {
        return value_;
    }

    /**
     * @brief Retrieves the name of the target type (e.g., "integer", "boolean").
     */
    const std::string& target_type() const noexcept {
        return target_type_;
    }

private:
    std::string value_;
    std::string target_type_;
};

/*
 * @brief Exception thrown when attempting to access a value that was not provided and has no default.
 * */
class ValueNotSet : public ParseError {
public:
    explicit ValueNotSet(const std::string& option_name)
        : ParseError("Value requested but was not set and has no default: " + option_name),
          option_name_(option_name) {}
    
    /**
     * @brief Retrieves the name of the option that was missing.
     */
    const std::string& option_name() const noexcept {
        return option_name_;
    }

private:
    std::string option_name_;

};

} // namespace cli
