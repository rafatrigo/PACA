#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstdlib>

// NOTE: We don't need to #include "parser.hpp" here because 
// this file is included AT THE BOTTOM of parser.hpp.

namespace cli {

// Use 'inline' to prevent multiple definition errors in a header-only library
inline Parser::Parser(std::string app_description)
    : description_(std::move(app_description)) {
        // Auto-register the default help flag
        add_flag('h', "help", "Print this help message and exit");
    }

inline void Parser::add_flag(char short_name, std::string_view long_name, std::string_view desc) {
    flags_.push_back({short_name, std::string(long_name), std::string(desc), false, nullptr});
}

inline void Parser::add_flag(char short_name, std::string_view long_name, std::string_view desc, bool& bound_var) {
    bound_var = false; 
    flags_.push_back({short_name, std::string(long_name), std::string(desc), false, &bound_var});
}

template <typename T>
inline Option<T>& Parser::add_option(std::string_view long_name) {
    auto opt = std::make_unique<Option<T>>(long_name);
    auto* ref = opt.get();
    options_.push_back(std::move(opt));
    return *ref;
}

template <typename T>
inline Option<T>& Parser::add_option(char short_name, std::string_view long_name) {
    auto opt = std::make_unique<Option<T>>(short_name, long_name);
    auto* ref = opt.get();
    options_.push_back(std::move(opt));
    return *ref;
}

//TODO: create TypeMismatch error and OptionNotFound error
template <typename T>
inline T Parser::get(std::string_view long_name) const {
    // check if it's a registered typed option
    for (const auto& opt_base : options_) {
        if (opt_base->long_name() == long_name) {
            auto* opt = dynamic_cast<const Option<T>*>(opt_base.get());
            if (!opt) {
                throw ParseError("Type mismatch when retrieving option: --" + std::string(long_name));
            }
            return opt->value().get();
        }
    }
    
    // check if the user is trying to get a simple boolean flag
    if constexpr (std::is_same_v<T, bool>) {
        for (const auto& flag : flags_) {
            if (flag.long_name == long_name) {
                return flag.internal_state;
            }
        }
    }
    
    throw ParseError("Option not found: --" + std::string(long_name));
}

inline void Parser::parse(int argc, const char* const* argv) {
    // Pre-scan for help flag. If found, print and exit immediately.
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            std::cout << generate_help();
            std::exit(0);
        }
    }

    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        //TODO: handle cases with '-' or '--' whithout flag/option
        if (arg.starts_with("--")) {
            if (arg.size() == 2) continue;          //ignoring this for now

            std::string_view param = arg.substr(2);
            auto eq_pos = param.find('=');

            if(eq_pos != std::string_view::npos) { // --key=value
                std::string_view key = param.substr(0, eq_pos);
                std::string_view value = param.substr(eq_pos + 1);

                if(!handle_long_option(key, value)) {
                    throw UnknownArgument(std::string(arg));
                }
            } else { // --key value OR --flag
                if(is_long_option(param)) {
                    if(i + 1 < argc) {
                        std::string_view value(argv[++i]);
                        if(!handle_long_option(param, value)) {
                            throw UnknownArgument(std::string(arg));
                        }
                    } else {
                        //TODO: make a MissingValue error
                        throw ParseError("Missing value for option: --" + std::string(param));
                    }
                } else {
                    handle_long_flag(param, arg);
                }
            }
        } else if (arg.starts_with("-")) {
            if (arg.size() == 1) continue;          //igniring this for now
            
            char short_name = arg[1];
            std::string_view param = arg.substr(1);
            auto eq_pos  = param.find('=');

            if(eq_pos != std::string_view::npos) { // -k=value
                std::string_view value = param.substr(eq_pos + 1);
                if(!handle_short_option(short_name, value)) {
                    throw UnknownArgument(std::string(arg));
                }
            } else { // -k value OR -f
                if(is_short_option(short_name)) {
                    if(i + 1 < argc) {
                        std::string_view value(argv[++i]);
                        if(!handle_short_option(short_name, value)) {
                            throw UnknownArgument(std::string(arg));
                        }
                    } else {
                        //TODO: make a MissingValue error
                        throw ParseError("Missing value for option: -" + std::string(param));
                    }
                } else {
                    handle_short_flag(short_name, arg);
                }
            }
            // NOTE: We will expand this later for grouped flags (e.g., -abc)
        } else {
            //NOTE: Positional arguments (Out of scope for v0.1, ignored for now)
        }
    }
}

inline bool Parser::is_set(std::string_view long_name) const {
    for (const auto& flag : flags_) {
        if (flag.long_name == long_name) return flag.internal_state;
    }
    return false;
}

inline void Parser::set_flag_state(Flag& flag) {
    flag.internal_state = true;
    if (flag.bound_value != nullptr) {
        *(flag.bound_value) = true;
    }
}

inline void Parser::handle_long_flag(std::string_view name, std::string_view original_arg) {
    for (auto& flag : flags_) {
        if (flag.long_name == name) {
            set_flag_state(flag);
            return;
        }
    }
    throw UnknownArgument(std::string(original_arg));
}

inline void Parser::handle_short_flag(char name, std::string_view original_arg) {
    for (auto& flag : flags_) {
        if (flag.short_name == name) {
            set_flag_state(flag);
            return;
        }
    }
    throw UnknownArgument(std::string(original_arg));
}

inline bool Parser::handle_long_option(std::string_view name, std::string_view value) {
    for (auto& opt : options_) {
        if (opt->long_name() == name) {
            opt->parse_and_set(value); // This routes to convert_to<T> internally
            return true;
        }
    }
    return false;
}

inline bool Parser::handle_short_option(char name, std::string_view value) {
    for (auto& opt : options_) {
        if (opt->short_name() == name) {
            opt->parse_and_set(value);
            return true;
        }
    }
    return false;
}

inline bool Parser::is_long_option(std::string_view name) const {
    for (const auto& opt : options_) {
        if (opt->long_name() == name) return true;
    }
    return false;
}

inline bool Parser::is_short_option(char name) const {
    for (const auto& opt : options_) {
        if (opt->short_name() == name) return true;
    }
    return false;
}

inline std::string Parser::generate_help() const {
    std::ostringstream oss;
    oss << "Usage: [options]\n\n";

    if (!description_.empty()) {
        oss << "Description:\n  " << description_ << "\n\n";
    }

    // Helper lambda to format the "-s, --long" part
    auto get_name_str = [](char short_name, std::string_view long_name) {
        std::string s = "  ";
        if (short_name != '\0') {
            s += "-";
            s += short_name;
            s += ", ";
        } else {
            s += "    "; // Padding if no short name exists
        }
        s += "--";
        s += long_name;
        return s;
    };

    // Calculate maximum width for formatting alignment
    size_t max_width = 0;
    for (const auto& flag : flags_) {
        max_width = std::max(max_width, get_name_str(flag.short_name, flag.long_name).length());
    }
    for (const auto& opt : options_) {
        max_width = std::max(max_width, get_name_str(opt->short_name(), opt->long_name()).length());
    }

    const int padding = 4; // Space between the option name and description
    oss << "Options:\n";

    // Print flags
    for (const auto& flag : flags_) {
        oss << std::left << std::setw(max_width + padding) 
            << get_name_str(flag.short_name, flag.long_name) 
            << flag.description << "\n";
    }

    // Print options
    for (const auto& opt : options_) {
        std::string desc = opt->description();
        if (opt->is_required()) {
            desc = "[Required] " + desc;
        }
        oss << std::left << std::setw(max_width + padding) 
            << get_name_str(opt->short_name(), opt->long_name()) 
            << desc << "\n";
    }

    return oss.str();
}
} // namespace cli
