#pragma once

#include <optional>
#include <stdexcept>
#include <utility>
#include <string_view>
#include "exceptions.hpp"

namespace cli {

/**
 * @brief Safely stores either a parsed command-line value or a fallback default.
 ** This class prioritizes explicitly parsed values. If no parsed value exists,
 * it falls back to the default value. If neither exists, accessing the value throws an error.
 */
template <typename T>
class Value {
public:
    /**
     * @brief Constructs a new Value, tracking the option's name for error reporting.
     * @param option_name The name of the option (e.g., "--port") used in exception messages.
     */
    explicit Value(std::string_view option_name) 
        : option_name_(option_name) {}

    /**
     * @brief Sets the explicitly parsed value from the command line.
     * @details This overrides any default value that may have been set.
     * @param val The parsed value to take ownership of.
     */
    void set(T val) {
        value_ = std::move(val);
    }

    /**
     * @brief Sets the fallback default value.
     * @param val The default value to use if no explicit value is parsed.
     */
    void set_default(T val) {
        default_value_ = std::move(val);
    }

    /**
     * @brief Checks if a value is safely available to read.
     * @return true if either a parsed value or a default value exists.
     */
    [[nodiscard]] bool has_value() const noexcept {
        return value_.has_value() || default_value_.has_value();
    }

    /**
     * @brief Retrieves the underlying value.
     * @return A const reference to the parsed value (prioritized) or the default value.
     * @throws cli::ValueNotSet if neither a parsed value nor a default value exists.
     */
    [[nodiscard]] const T& get() const {
        if (value_.has_value()) {
            return value_.value();
        }
        if (default_value_.has_value()) {
            return default_value_.value();
        }
        throw cli::ValueNotSet(option_name_);
    }

private:
    std::optional<T> value_;
    std::optional<T> default_value_;
    std::string option_name_;
};

} // namespace cli
