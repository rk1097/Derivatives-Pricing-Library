#pragma once

#include "pricing_model.hpp"
#include "../math/random.hpp"
#include <vector>
#include <memory>

namespace derivatives {

// Configuration for Monte Carlo simulation
struct MonteCarloConfig {
    size_t num_paths;           // Number of simulation paths
    size_t num_timesteps;       // Number of time steps per path
    unsigned int seed;          // Random seed
    bool use_antithetic;        // Use antithetic variates for variance reduction
    bool use_control_variate;   // Use control variate technique

    MonteCarloConfig()
        : num_paths(100000), num_timesteps(100), seed(12345),
          use_antithetic(true), use_control_variate(false) {}
};

// Monte Carlo pricing engine
class MonteCarloModel : public PricingModel {
public:
    explicit MonteCarloModel(const MonteCarloConfig& config = MonteCarloConfig())
        : config_(config), rng_(config.seed) {}

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    // Get configuration
    const MonteCarloConfig& config() const { return config_; }
    void set_config(const MonteCarloConfig& config) {
        config_ = config;
        rng_.set_seed(config.seed);
    }

    // Simulate a single path of the underlying asset price
    std::vector<double> simulate_path(double S0, const MarketData& market_data,
                                     double T, size_t num_steps) const;

    // Price Asian option using Monte Carlo
    double price_asian(const class AsianOption& option, const MarketData& market_data) const;

    // Price barrier option using Monte Carlo
    double price_barrier(const class BarrierOption& option, const MarketData& market_data) const;

private:
    MonteCarloConfig config_;
    mutable math::RandomGenerator rng_;

    // Calculate discounted payoff for a given path
    double calculate_payoff(const Option& option, const std::vector<double>& path) const;
};

} // namespace derivatives
