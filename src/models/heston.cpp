#include "../../include/derivatives/models/heston.hpp"
#include "../../include/derivatives/math/statistics.hpp"
#include <cmath>
#include <algorithm>

namespace derivatives {

double HestonModel::price(const Option& option, const MarketData& market_data) const {
    if (option.style() != OptionStyle::EUROPEAN) {
        throw std::invalid_argument("Heston model currently only supports European options");
    }

    // Use semi-analytical pricing for better accuracy
    return price_semi_analytical(option, market_data);
}

double HestonModel::price_semi_analytical(const Option& option, const MarketData& market_data) const {
    double S = market_data.spot;
    double K = option.strike();
    double T = option.expiry();
    double r = market_data.rate;
    double q = market_data.dividend;

    // Calculate P1 and P2 using characteristic function
    double P1 = 0.5 + integrate(S, K, T, r, q, 1) / M_PI;
    double P2 = 0.5 + integrate(S, K, T, r, q, 2) / M_PI;

    if (option.type() == OptionType::CALL) {
        return S * std::exp(-q * T) * P1 - K * std::exp(-r * T) * P2;
    } else {
        return K * std::exp(-r * T) * (1.0 - P2) - S * std::exp(-q * T) * (1.0 - P1);
    }
}

std::complex<double> HestonModel::characteristic_function(std::complex<double> u,
                                                         double S, double v, double T,
                                                         double r, double q) const {
    using namespace std::complex_literals;

    std::complex<double> i(0.0, 1.0);

    double kappa = params_.kappa;
    double theta = params_.theta;
    double sigma = params_.sigma;
    double rho = params_.rho;

    std::complex<double> d = std::sqrt(
        std::pow(rho * sigma * u * i - kappa, 2.0) +
        sigma * sigma * (u * i + u * u)
    );

    std::complex<double> g = (kappa - rho * sigma * u * i - d) /
                            (kappa - rho * sigma * u * i + d);

    std::complex<double> C = (r - q) * u * i * T +
                            (kappa * theta) / (sigma * sigma) * (
                                (kappa - rho * sigma * u * i - d) * T -
                                2.0 * std::log((1.0 - g * std::exp(-d * T)) / (1.0 - g))
                            );

    std::complex<double> D = (kappa - rho * sigma * u * i - d) / (sigma * sigma) *
                            (1.0 - std::exp(-d * T)) / (1.0 - g * std::exp(-d * T));

    return std::exp(C + D * v + i * u * std::log(S));
}

double HestonModel::heston_integrand(double phi, double S, double K, double T,
                                    double r, double q, int j) const {
    using namespace std::complex_literals;
    std::complex<double> i(0.0, 1.0);
    std::complex<double> u;

    if (j == 1) {
        u = phi - i;
    } else {
        u = phi;
    }

    std::complex<double> f = characteristic_function(u, S, params_.v0, T, r, q);
    std::complex<double> integrand = std::exp(-i * phi * std::log(K)) * f / (i * phi);

    return integrand.real();
}

double HestonModel::integrate(double S, double K, double T, double r, double q, int j) const {
    // Simple numerical integration using trapezoidal rule
    const int n_points = 1000;
    const double upper_limit = 100.0;
    double dx = upper_limit / n_points;

    double sum = 0.0;
    for (int i = 1; i < n_points; ++i) {
        double phi = i * dx;
        sum += heston_integrand(phi, S, K, T, r, q, j);
    }

    return sum * dx;
}

double HestonModel::price_monte_carlo(const Option& option, const MarketData& market_data,
                                     size_t num_paths, size_t num_steps) const {
    double S0 = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;

    math::RandomGenerator rng(12345);
    double sum = 0.0;

    for (size_t i = 0; i < num_paths; ++i) {
        auto [S_path, v_path] = simulate_path(S0, params_.v0, market_data, T, num_steps, rng);
        double payoff = option.payoff(S_path.back());
        sum += payoff;
    }

    double mean_payoff = sum / num_paths;
    return std::exp(-r * T) * mean_payoff;
}

std::pair<std::vector<double>, std::vector<double>>
HestonModel::simulate_path(double S0, double v0, const MarketData& market_data,
                          double T, size_t num_steps, math::RandomGenerator& rng) const {
    std::vector<double> S_path(num_steps + 1);
    std::vector<double> v_path(num_steps + 1);

    S_path[0] = S0;
    v_path[0] = v0;

    double dt = T / num_steps;
    double r = market_data.rate;
    double q = market_data.dividend;

    for (size_t i = 1; i <= num_steps; ++i) {
        auto [Z1, Z2] = rng.correlated_normals(params_.rho);

        // Variance process (ensure non-negative using truncation scheme)
        double v_prev = std::max(v_path[i - 1], 0.0);
        double dv = params_.kappa * (params_.theta - v_prev) * dt +
                   params_.sigma * std::sqrt(v_prev * dt) * Z2;
        v_path[i] = v_prev + dv;

        // Asset price process
        double dS = (r - q) * S_path[i - 1] * dt +
                   S_path[i - 1] * std::sqrt(v_prev * dt) * Z1;
        S_path[i] = S_path[i - 1] + dS;
    }

    return {S_path, v_path};
}

Greeks HestonModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

} // namespace derivatives
