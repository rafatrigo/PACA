#pragma once

#include <string>
#include <string_view>
#include "value.hpp"
#include "detail/convert_impl.hpp"

namespace cli {

/**
 * @brief Type-erased base class for all options.
 * @details Allows the Parser to store a heterogeneous collection of options 
 * (e.g., mixing Option<int> and Option<std::string> in a single container OptionBase) 
 * without making the Parser itself a template.
 */
class OptionBase {
public:
    /**
     * @brief Constructs a base option with only a long name.
     * @param long_name The long identifier for the option (e.g., "port").
     */
    explicit OptionBase(std::string_view long_name)
        : long_name_(long_name) {}

    /**
     * @brief Constructs a base option with both a short and long name.
     * @param short_name The single-character short identifier (e.g., 'p').
     * @param long_name The long identifier for the option (e.g., "port").
     */
    OptionBase(char short_name, std::string_view long_name)
        : short_name_(short_name), long_name_(long_name) {}

    virtual ~OptionBase() = default;

    /**
     * @brief Type-erased interface to trigger string-to-type conversion.
     * @details The Parser calls this method with the raw string from the command line,
     * delegating the actual type conversion to the strongly-typed derived class.
     * @param str_value The raw string extracted from the command-line arguments.
     */
    virtual void parse_and_set(std::string_view str_value) = 0;

    /** @return The short name character, or '\0' if not set. */
    [[nodiscard]] char short_name() const noexcept { return short_name_; }

    /** @return The long name string (e.g., "port"). */
    [[nodiscard]] const std::string& long_name() const noexcept { return long_name_; }

    /** @return The help description assigned to this option. */
    [[nodiscard]] const std::string& description() const noexcept { return description_; }

    /** @return true if the user must provide this option to parse successfully. */
    [[nodiscard]] bool is_required() const noexcept { return required_; }

protected:
    char short_name_{'\0'};
    std::string long_name_;
    std::string description_;
    bool required_{false};
};

/**
 * @brief Strongly-typed option implementing a fluent builder pattern.
 * @tparam T the expected C++ data type for this option (e.g., int, std::string, bool).
 */
template <typename T>
class Option final : public OptionBase {
public:
    using OptionBase::description;

    /**
     * @brief Constructs a strongly-typed option with a long name.
     * @param long_name The long identifier (e.g., "port").
     */
    explicit Option(std::string_view long_name)
        : OptionBase(long_name), 
          value_(long_name) {}

    /**
     * @brief Constructs a strongly-typed option with both a short and long name.
     * @param short_name The single-character short identifier (e.g., 'p').
     * @param long_name The long identifier (e.g., "port").
     */
    Option(char short_name, std::string_view long_name)
        : OptionBase(short_name, long_name), 
          value_(long_name) {} // <-- And here

    // ------------------------------------------------------------------------
    // Builder API
    // ------------------------------------------------------------------------

    /**
     * @brief Sets the fallback default value for this option.
     * @param val The value to use if the user does not provide the option on the command line.
     * @return A reference to *this to allow method chaining.
     */
    Option& default_value(T val) {
        value_.set_default(std::move(val));
        return *this;
    }
    
    /**
     * @brief Sets the description shown in the help menu.
     * @param desc A brief explanation of what the option does.
     * @return A reference to *this to allow method chaining.
     */
    Option& description(std::string_view desc) {
        description_ = std::string(desc);
        return *this;
    }
    
    /**
     * @brief Marks this option as strictly required.
     * @return A reference to *this to allow method chaining.
     */
    Option& required() {
        required_ = true;
        return *this;
    }

    // ------------------------------------------------------------------------
    // Parser Interface
    // ------------------------------------------------------------------------

    /**
     * @brief Parses the raw string into type T and sets the internal value.
     * @param str_value The raw string to parse.
     * @throws cli::ConversionError if the string cannot be converted to type T.
     */
    void parse_and_set(std::string_view str_value) override {
        value_.set(cli::detail::convert_to<T>(str_value));
    }

    // ------------------------------------------------------------------------
    // User Access
    // ------------------------------------------------------------------------

    /**
     * @brief Retrieves the underlying safe Value container.
     * @return A const reference to the Value<T> object holding the parsed/default state.
     */
    [[nodiscard]] const Value<T>& value() const noexcept {
        return value_;
    }

private:
    Value<T> value_;
};

} // namespace cli
