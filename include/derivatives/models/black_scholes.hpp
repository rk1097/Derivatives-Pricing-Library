#pragma once

#include "pricing_model.hpp"
#include "../math/statistics.hpp"
#include <cmath>

namespace derivatives {

// Black-Scholes model for European options
class BlackScholesModel : public PricingModel {
public:
    BlackScholesModel() = default;

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

    // Price European call option
    double call_price(double S, double K, double T, double r, double q, double sigma) const;

    // Price European put option
    double put_price(double S, double K, double T, double r, double q, double sigma) const;

    // Implied volatility calculation using Newton-Raphson
    double implied_volatility(double market_price, const Option& option,
                            const MarketData& market_data, double initial_guess = 0.3,
                            double tolerance = 1e-6, int max_iterations = 100) const;

private:
    double calculate_d1(double S, double K, double T, double r, double q, double sigma) const {
        return (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    }

    double calculate_d2(double d1, double sigma, double T) const {
        return d1 - sigma * std::sqrt(T);
    }
};

// Black-Scholes for Digital Options
class BlackScholesDigitalModel : public PricingModel {
public:
    BlackScholesDigitalModel() = default;

    double price(const Option& option, const MarketData& market_data) const override;
    Greeks greeks(const Option& option, const MarketData& market_data) const override;

private:
    double calculate_d1(double S, double K, double T, double r, double q, double sigma) const {
        return (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    }

    double calculate_d2(double d1, double sigma, double T) const {
        return d1 - sigma * std::sqrt(T);
    }
};

} // namespace derivatives
