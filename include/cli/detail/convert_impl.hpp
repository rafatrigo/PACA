#pragma once

#include <string>
#include <string_view>
#include <charconv>
#include <system_error>
#include <algorithm>
#include <cctype>
#include <type_traits>
#include <array>

#include "cli/exceptions.hpp"

namespace cli::detail {

/**
 * @brief Primary template for type conversion. 
 * Users can specialize this for their own custom types.
 */
template <typename T>
struct Converter {
    static T convert(std::string_view value) {
        // By default, if an unsupported type is used, compilation will fail here
        // or a generic stringstream fallback could be placed. 
        // For now, we enforce specialization.
        static_assert(sizeof(T) == 0, "No Converter specialization found for this type.");
        return T{};
    }
};

// ----------------------------------------------------------------------------
// Specialization: std::string
// ----------------------------------------------------------------------------
template <>
struct Converter<std::string> {
    static std::string convert(std::string_view value) {
        return std::string(value);
    }
};

// ----------------------------------------------------------------------------
// Internal Helper for all numeric types
// ----------------------------------------------------------------------------
template <typename T>
T convert_numeric(std::string_view value, const char* type_name) {
    // Prevent accidental misuse of this helper for non-numeric types
    static_assert(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, 
                  "convert_numeric is only for numbers");

    if (value.empty()) {
        throw ConversionError(std::string(value), type_name);
    }

    T result{};
    const char* first = value.data();
    const char* last = value.data() + value.size();
    
    auto [ptr, ec] = std::from_chars(first, last, result);

    if (ec != std::errc() || ptr != last) {
        throw ConversionError(std::string(value), type_name);
    }

    return result;
}

// ----------------------------------------------------------------------------
// Numeric Specializations
// ----------------------------------------------------------------------------
template <>
struct Converter<int> {
    static int convert(std::string_view value) {
        return convert_numeric<int>(value, "int");
    }
};

template <>
struct Converter<double> {
    static double convert(std::string_view value) {
        return convert_numeric<double>(value, "double");
    }
};

template <>
struct Converter<float> {
    static float convert(std::string_view value) {
        return convert_numeric<float>(value, "float");
    }
};

// ----------------------------------------------------------------------------
// Specialization: bool
// ----------------------------------------------------------------------------
template <>
struct Converter<bool> {
    static constexpr std::array<std::string_view, 5> truthy = {"true", "1", "yes", "y", "on"};
    static constexpr std::array<std::string_view, 5> falsy  = {"false", "0", "no", "n", "off"};

    static bool convert(std::string_view value) {
        if (value.empty()) {
            throw ConversionError(std::string(value), "bool");
        }

        // Convert to lowercase for case-insensitive matching
        std::string lower_str(value);
        std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        // Check truthy values
        if (std::find(truthy.begin(), truthy.end(), lower_str) != truthy.end()) {
            return true;
        }

        // Check falsy values
        if (std::find(falsy.begin(), falsy.end(), lower_str) != falsy.end()) {
            return false;
        }

        throw ConversionError(std::string(value), "bool");
    }
};

// Helper function to simplify calling the converter
template <typename T>
T convert_to(std::string_view value) {
    return Converter<T>::convert(value);
}

} // namespace cli::detail
