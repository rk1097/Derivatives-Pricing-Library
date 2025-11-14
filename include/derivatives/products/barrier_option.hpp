#pragma once

#include "option.hpp"

namespace derivatives {

// Barrier option - option that is activated or deactivated when barrier is hit
class BarrierOption : public Option {
public:
    BarrierOption(double strike, double expiry, OptionType type,
                  BarrierType barrier_type, double barrier_level,
                  double rebate = 0.0)
        : Option(strike, expiry, type, OptionStyle::EUROPEAN),
          barrier_type_(barrier_type),
          barrier_level_(barrier_level),
          rebate_(rebate) {
        validate_barrier();
    }

    BarrierType barrier_type() const { return barrier_type_; }
    double barrier_level() const { return barrier_level_; }
    double rebate() const { return rebate_; }

    // Check if barrier has been breached
    bool is_knocked(double spot_price) const {
        switch (barrier_type_) {
            case BarrierType::UP_AND_IN:
            case BarrierType::UP_AND_OUT:
                return spot_price >= barrier_level_;
            case BarrierType::DOWN_AND_IN:
            case BarrierType::DOWN_AND_OUT:
                return spot_price <= barrier_level_;
            default:
                return false;
        }
    }

    // Check if it's a knock-in option
    bool is_knock_in() const {
        return barrier_type_ == BarrierType::UP_AND_IN ||
               barrier_type_ == BarrierType::DOWN_AND_IN;
    }

    // Check if it's a knock-out option
    bool is_knock_out() const {
        return barrier_type_ == BarrierType::UP_AND_OUT ||
               barrier_type_ == BarrierType::DOWN_AND_OUT;
    }

private:
    BarrierType barrier_type_;
    double barrier_level_;
    double rebate_;  // Payment if barrier is breached (for knock-out) or not breached (for knock-in)

    void validate_barrier() const {
        if (!std::isfinite(barrier_level_) || barrier_level_ <= 0.0) {
            throw std::invalid_argument("Barrier level must be positive and finite");
        }
        if (!std::isfinite(rebate_) || rebate_ < 0.0) {
            throw std::invalid_argument("Rebate must be non-negative and finite");
        }
    }
};

} // namespace derivatives
