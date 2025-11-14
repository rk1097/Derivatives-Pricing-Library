#pragma once

#include "pricing_model.hpp"
#include "../math/random.hpp"
#include <complex>

namespace derivatives {

// Heston model parameters
struct HestonParams {
    double kappa;       // Mean reversion speed
    double theta;       // Long-term variance
    double sigma;       // Volatility of volatility
    double rho;         // Correlation between asset and variance
    double v0;          // Initial variance

    HestonParams()
        : kappa(2.0), theta(0.04), sigma(0.3), rho(-0.7), v0(0.04) {}

    HestonParams(double k, double t, double s, double r, double v)
        : kappa(k), theta(t), sigma(s), rho(r), v0(v) {}
};

// Heston stochastic volatility model
class HestonModel : public PricingModel {
public:
    HestonModel(const HestonParams& params = HestonParams())
        : params_(params) {}

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    // Semi-analytical pricing using characteristic function
    double price_semi_analytical(const Option& option, const MarketData& market_data) const;

    // Monte Carlo pricing with Heston dynamics
    double price_monte_carlo(const Option& option, const MarketData& market_data,
                            size_t num_paths = 100000, size_t num_steps = 100) const;

    const HestonParams& params() const { return params_; }
    void set_params(const HestonParams& p) { params_ = p; }

private:
    HestonParams params_;

    // Characteristic function for Heston model
    std::complex<double> characteristic_function(std::complex<double> u, double S, double v,
                                                double T, double r, double q) const;

    // Heston integrand for pricing
    double heston_integrand(double phi, double S, double K, double T, double r,
                           double q, int j) const;

    // Numerical integration (using Simpson's rule or similar)
    double integrate(double S, double K, double T, double r, double q, int j) const;

    // Simulate one path with Heston dynamics
    std::pair<std::vector<double>, std::vector<double>>
    simulate_path(double S0, double v0, const MarketData& market_data,
                 double T, size_t num_steps, math::RandomGenerator& rng) const;
};

} // namespace derivatives
