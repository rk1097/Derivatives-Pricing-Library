#include "../../include/derivatives/models/black_scholes.hpp"
#include "../../include/derivatives/products/digital_option.hpp"
#include <cmath>
#include <stdexcept>

namespace derivatives {

double BlackScholesModel::price(const Option& option, const MarketData& market_data) const {
    double S = market_data.spot;
    double K = option.strike();
    double T = option.expiry();
    double r = market_data.rate;
    double q = market_data.dividend;
    double sigma = market_data.volatility;

    if (option.style() != OptionStyle::EUROPEAN) {
        throw std::invalid_argument("Black-Scholes model only supports European options");
    }

    if (option.type() == OptionType::CALL) {
        return call_price(S, K, T, r, q, sigma);
    } else {
        return put_price(S, K, T, r, q, sigma);
    }
}

double BlackScholesModel::call_price(double S, double K, double T,
                                    double r, double q, double sigma) const {
    double d1 = calculate_d1(S, K, T, r, q, sigma);
    double d2 = calculate_d2(d1, sigma, T);
    return S * std::exp(-q * T) * math::norm_cdf(d1) - K * std::exp(-r * T) * math::norm_cdf(d2);
}

double BlackScholesModel::put_price(double S, double K, double T,
                                   double r, double q, double sigma) const {
    double d1 = calculate_d1(S, K, T, r, q, sigma);
    double d2 = calculate_d2(d1, sigma, T);
    return K * std::exp(-r * T) * math::norm_cdf(-d2) - S * std::exp(-q * T) * math::norm_cdf(-d1);
}

Greeks BlackScholesModel::greeks(const Option& option, const MarketData& market_data) const {
    double S = market_data.spot;
    double K = option.strike();
    double T = option.expiry();
    double r = market_data.rate;
    double q = market_data.dividend;
    double sigma = market_data.volatility;

    double d1 = calculate_d1(S, K, T, r, q, sigma);
    double d2 = calculate_d2(d1, sigma, T);
    double Nd1 = math::norm_cdf(d1);
    double Nd2 = math::norm_cdf(d2);
    double npd1 = math::norm_pdf(d1);
    double sqrt_T = std::sqrt(T);
    double exp_qT = std::exp(-q * T);
    double exp_rT = std::exp(-r * T);

    Greeks g;

    if (option.type() == OptionType::CALL) {
        g.delta = exp_qT * Nd1;
        g.gamma = exp_qT * npd1 / (S * sigma * sqrt_T);
        g.vega = S * exp_qT * npd1 * sqrt_T / 100.0;

        double theta_annual = -S * npd1 * sigma * exp_qT / (2.0 * sqrt_T)
                              - r * K * exp_rT * Nd2
                              + q * S * exp_qT * Nd1;
        g.theta = theta_annual / 365.0;

        g.rho = K * T * exp_rT * Nd2 / 100.0;
    } else {
        double N_minus_d1 = math::norm_cdf(-d1);
        double N_minus_d2 = math::norm_cdf(-d2);

        g.delta = exp_qT * (Nd1 - 1.0);
        g.gamma = exp_qT * npd1 / (S * sigma * sqrt_T);
        g.vega = S * exp_qT * npd1 * sqrt_T / 100.0;

        double theta_annual = -S * npd1 * sigma * exp_qT / (2.0 * sqrt_T)
                              + r * K * exp_rT * N_minus_d2
                              - q * S * exp_qT * N_minus_d1;
        g.theta = theta_annual / 365.0;

        g.rho = -K * T * exp_rT * N_minus_d2 / 100.0;
    }

    return g;
}

double BlackScholesModel::implied_volatility(double market_price, const Option& option,
                                            const MarketData& market_data,
                                            double initial_guess, double tolerance,
                                            int max_iterations) const {
    double sigma = initial_guess;
    MarketData temp_data = market_data;

    for (int i = 0; i < max_iterations; ++i) {
        temp_data.volatility = sigma;
        double price = this->price(option, temp_data);
        double diff = price - market_price;

        if (std::abs(diff) < tolerance) {
            return sigma;
        }

        // Vega for Newton-Raphson
        Greeks g = greeks(option, temp_data);
        double vega = g.vega * 100.0;  // Convert from percentage

        if (std::abs(vega) < 1e-10) {
            throw std::runtime_error("Vega too small, cannot compute implied volatility");
        }

        sigma = sigma - diff / vega;

        if (sigma <= 0.0) {
            sigma = initial_guess * 0.5;
        }
    }

    throw std::runtime_error("Implied volatility did not converge");
}

// Digital option pricing
double BlackScholesDigitalModel::price(const Option& option, const MarketData& market_data) const {
    const DigitalOption* digital = dynamic_cast<const DigitalOption*>(&option);
    if (!digital) {
        throw std::invalid_argument("Option must be a DigitalOption");
    }

    double S = market_data.spot;
    double K = option.strike();
    double T = option.expiry();
    double r = market_data.rate;
    double q = market_data.dividend;
    double sigma = market_data.volatility;

    double d1 = calculate_d1(S, K, T, r, q, sigma);
    double d2 = calculate_d2(d1, sigma, T);

    double payout = digital->payout();

    if (option.type() == OptionType::CALL) {
        return payout * std::exp(-r * T) * math::norm_cdf(d2);
    } else {
        return payout * std::exp(-r * T) * math::norm_cdf(-d2);
    }
}

Greeks BlackScholesDigitalModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

} // namespace derivatives
