#pragma once

#include "pricing_model.hpp"
#include <vector>
#include <algorithm>

namespace derivatives {

// Binomial tree model for option pricing (supports American options)
class BinomialTreeModel : public PricingModel {
public:
    explicit BinomialTreeModel(size_t num_steps = 100)
        : num_steps_(num_steps) {}

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    void set_num_steps(size_t n) { num_steps_ = n; }
    size_t num_steps() const { return num_steps_; }

private:
    size_t num_steps_;

    // Cox-Ross-Rubinstein (CRR) parameterization
    struct TreeParams {
        double u;    // Up movement factor
        double d;    // Down movement factor
        double p;    // Risk-neutral probability of up movement
        double dt;   // Time step
        double df;   // Discount factor per step
    };

    TreeParams calculate_parameters(const MarketData& market_data, double T) const;

    double price_european(const Option& option, const MarketData& market_data,
                         const TreeParams& params) const;

    double price_american(const Option& option, const MarketData& market_data,
                         const TreeParams& params) const;
};

// Trinomial tree model (more accurate for some cases)
class TrinomialTreeModel : public PricingModel {
public:
    explicit TrinomialTreeModel(size_t num_steps = 100)
        : num_steps_(num_steps) {}

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    void set_num_steps(size_t n) { num_steps_ = n; }

private:
    size_t num_steps_;
};

} // namespace derivatives
