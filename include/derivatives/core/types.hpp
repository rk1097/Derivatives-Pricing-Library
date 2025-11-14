#pragma once

#include <string>
#include <stdexcept>

namespace derivatives {

// Enumeration for option types
enum class OptionType {
    CALL,
    PUT
};

// Enumeration for option styles
enum class OptionStyle {
    EUROPEAN,
    AMERICAN,
    BERMUDAN
};

// Enumeration for barrier types
enum class BarrierType {
    UP_AND_IN,
    UP_AND_OUT,
    DOWN_AND_IN,
    DOWN_AND_OUT
};

// Enumeration for averaging types (for Asian options)
enum class AveragingType {
    ARITHMETIC,
    GEOMETRIC
};

// Convert option type to string
inline std::string to_string(OptionType type) {
    return type == OptionType::CALL ? "CALL" : "PUT";
}

// Convert option style to string
inline std::string to_string(OptionStyle style) {
    switch(style) {
        case OptionStyle::EUROPEAN: return "EUROPEAN";
        case OptionStyle::AMERICAN: return "AMERICAN";
        case OptionStyle::BERMUDAN: return "BERMUDAN";
        default: return "UNKNOWN";
    }
}

} // namespace derivatives
