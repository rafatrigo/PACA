#pragma once
// NOTE: We don't need to #include "parser.hpp" here because 
// this file is included AT THE BOTTOM of parser.hpp.

namespace cli {

// Use 'inline' to prevent multiple definition errors in a header-only library
inline Parser::Parser(std::string app_description)
    : description_(std::move(app_description)) {}

inline void Parser::add_flag(char short_name, std::string_view long_name, std::string_view desc) {
    flags_.push_back({short_name, std::string(long_name), std::string(desc), false, nullptr});
}

inline void Parser::add_flag(char short_name, std::string_view long_name, std::string_view desc, bool& bound_var) {
    bound_var = false; 
    flags_.push_back({short_name, std::string(long_name), std::string(desc), false, &bound_var});
}

inline void Parser::parse(int argc, const char* const* argv) {
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        //TODO: handle cases with '-' or '--' whithout flag
        if (arg.starts_with("--")) {
            if (arg.size() == 2) continue;          //ignoring this for now
            handle_long_flag(arg.substr(2), arg);
        } else if (arg.starts_with("-")) {
            if (arg.size() == 1) continue;          //igniring this for now
            // NOTE: We will expand this later for grouped flags (e.g., -abc)
            handle_short_flag(arg[1], arg);
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

} // namespace cli
