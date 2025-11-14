#pragma once

#include "../core/types.hpp"
#include <stdexcept>
#include <cmath>

namespace derivatives {

// Base class for all options
class Option {
public:
    Option(double strike, double expiry, OptionType type, OptionStyle style = OptionStyle::EUROPEAN)
        : strike_(strike), expiry_(expiry), type_(type), style_(style) {
        validate();
    }

    virtual ~Option() = default;

    // Getters
    double strike() const { return strike_; }
    double expiry() const { return expiry_; }
    OptionType type() const { return type_; }
    OptionStyle style() const { return style_; }

    // Payoff function
    virtual double payoff(double spot) const {
        if (type_ == OptionType::CALL) {
            return std::max(spot - strike_, 0.0);
        } else {
            return std::max(strike_ - spot, 0.0);
        }
    }

protected:
    double strike_;
    double expiry_;
    OptionType type_;
    OptionStyle style_;

    void validate() const {
        if (!std::isfinite(strike_) || strike_ <= 0.0) {
            throw std::invalid_argument("Strike price must be positive and finite");
        }
        if (!std::isfinite(expiry_) || expiry_ <= 0.0) {
            throw std::invalid_argument("Expiry must be positive and finite");
        }
    }
};

// European vanilla option
class EuropeanOption : public Option {
public:
    EuropeanOption(double strike, double expiry, OptionType type)
        : Option(strike, expiry, type, OptionStyle::EUROPEAN) {}
};

// American option (can be exercised any time before expiry)
class AmericanOption : public Option {
public:
    AmericanOption(double strike, double expiry, OptionType type)
        : Option(strike, expiry, type, OptionStyle::AMERICAN) {}
};

} // namespace derivatives
