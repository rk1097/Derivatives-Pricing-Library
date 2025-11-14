#pragma once

#include "option.hpp"

namespace derivatives {

// Digital (Binary) option - pays fixed amount if condition is met, zero otherwise
class DigitalOption : public Option {
public:
    DigitalOption(double strike, double expiry, OptionType type, double payout = 1.0)
        : Option(strike, expiry, type, OptionStyle::EUROPEAN),
          payout_(payout) {
        if (!std::isfinite(payout) || payout <= 0.0) {
            throw std::invalid_argument("Payout must be positive and finite");
        }
    }

    double payout() const { return payout_; }

    // Digital option payoff - either fixed payout or zero
    double payoff(double spot) const override {
        bool in_the_money = (type_ == OptionType::CALL) ? (spot > strike_) : (spot < strike_);
        return in_the_money ? payout_ : 0.0;
    }

private:
    double payout_;  // Fixed payout amount
};

// Cash-or-nothing digital option
class CashOrNothingOption : public DigitalOption {
public:
    CashOrNothingOption(double strike, double expiry, OptionType type, double cash_amount)
        : DigitalOption(strike, expiry, type, cash_amount) {}
};

// Asset-or-nothing digital option (pays asset value if in the money)
class AssetOrNothingOption : public Option {
public:
    AssetOrNothingOption(double strike, double expiry, OptionType type)
        : Option(strike, expiry, type, OptionStyle::EUROPEAN) {}

    // Payoff is the asset price if in the money, zero otherwise
    double payoff(double spot) const override {
        bool in_the_money = (type_ == OptionType::CALL) ? (spot > strike_) : (spot < strike_);
        return in_the_money ? spot : 0.0;
    }
};

} // namespace derivatives
