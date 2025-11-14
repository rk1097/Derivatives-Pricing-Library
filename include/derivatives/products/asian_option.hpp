#pragma once

#include "option.hpp"
#include <vector>

namespace derivatives {

// Asian option - payoff depends on average price over some time period
class AsianOption : public Option {
public:
    AsianOption(double strike, double expiry, OptionType type,
                AveragingType avg_type = AveragingType::ARITHMETIC,
                size_t num_observations = 12)
        : Option(strike, expiry, type, OptionStyle::EUROPEAN),
          averaging_type_(avg_type),
          num_observations_(num_observations) {}

    AveragingType averaging_type() const { return averaging_type_; }
    size_t num_observations() const { return num_observations_; }

    // Calculate average from a series of observations
    double calculate_average(const std::vector<double>& observations) const {
        if (observations.empty()) return 0.0;

        if (averaging_type_ == AveragingType::ARITHMETIC) {
            double sum = 0.0;
            for (double obs : observations) {
                sum += obs;
            }
            return sum / observations.size();
        } else {
            // Geometric average
            double product = 1.0;
            for (double obs : observations) {
                product *= obs;
            }
            return std::pow(product, 1.0 / observations.size());
        }
    }

    // Payoff based on average price
    double payoff(double average_price) const override {
        if (type_ == OptionType::CALL) {
            return std::max(average_price - strike_, 0.0);
        } else {
            return std::max(strike_ - average_price, 0.0);
        }
    }

private:
    AveragingType averaging_type_;
    size_t num_observations_;
};

} // namespace derivatives
