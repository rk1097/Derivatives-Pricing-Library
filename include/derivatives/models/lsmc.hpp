#pragma once

#include "pricing_model.hpp"
#include "../math/random.hpp"
#include "../math/matrix.hpp"
#include <vector>
#include <memory>

namespace derivatives {

// Longstaff-Schwartz Monte Carlo for American options
class LSMCModel : public PricingModel {
public:
    struct Config {
        size_t num_paths;           // Number of Monte Carlo paths
        size_t num_timesteps;       // Number of time steps
        unsigned int seed;          // Random seed
        bool use_antithetic;        // Use antithetic variates
        int polynomial_degree;      // Degree of polynomial for regression

        Config()
            : num_paths(50000), num_timesteps(50), seed(12345),
              use_antithetic(true), polynomial_degree(3) {}
    };

    explicit LSMCModel(const Config& config = Config())
        : config_(config), rng_(config.seed) {}

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    const Config& config() const { return config_; }
    void set_config(const Config& cfg) {
        config_ = cfg;
        rng_.set_seed(cfg.seed);
    }

private:
    Config config_;
    mutable math::RandomGenerator rng_;

    // Generate all price paths
    std::vector<std::vector<double>> generate_paths(double S0, const MarketData& market_data,
                                                    double T) const;

    // Perform least squares regression to estimate continuation value
    std::vector<double> regression(const std::vector<double>& x,
                                   const std::vector<double>& y,
                                   int degree) const;

    // Evaluate polynomial at point x given coefficients
    double evaluate_polynomial(const std::vector<double>& coeffs, double x) const;

    // Generate basis functions (Laguerre polynomials)
    std::vector<double> basis_functions(double x, int degree) const;
};

} // namespace derivatives
