#include "../../include/derivatives/models/monte_carlo.hpp"
#include "../../include/derivatives/products/asian_option.hpp"
#include "../../include/derivatives/products/barrier_option.hpp"
#include <cmath>
#include <algorithm>

namespace derivatives {

std::vector<double> MonteCarloModel::simulate_path(double S0, const MarketData& market_data,
                                                   double T, size_t num_steps) const {
    std::vector<double> path(num_steps + 1);
    path[0] = S0;

    double dt = T / num_steps;
    double sigma = market_data.volatility;
    double r = market_data.rate;
    double q = market_data.dividend;
    double drift = (r - q - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);

    for (size_t i = 1; i <= num_steps; ++i) {
        double dW = rng_.next_normal();
        path[i] = path[i - 1] * std::exp(drift + diffusion * dW);
    }

    return path;
}

double MonteCarloModel::price(const Option& option, const MarketData& market_data) const {
    if (option.style() != OptionStyle::EUROPEAN) {
        throw std::invalid_argument("Basic Monte Carlo only supports European options. Use LSMC for American options.");
    }

    // Check for specific option types
    const AsianOption* asian = dynamic_cast<const AsianOption*>(&option);
    if (asian) {
        return price_asian(*asian, market_data);
    }

    const BarrierOption* barrier = dynamic_cast<const BarrierOption*>(&option);
    if (barrier) {
        return price_barrier(*barrier, market_data);
    }

    // Standard European option
    double S0 = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;

    double sum = 0.0;
    double sum_sq = 0.0;

    size_t total_paths = config_.use_antithetic ? config_.num_paths / 2 : config_.num_paths;

    for (size_t i = 0; i < total_paths; ++i) {
        auto path = simulate_path(S0, market_data, T, config_.num_timesteps);
        double S_T = path.back();
        double payoff = option.payoff(S_T);
        sum += payoff;
        sum_sq += payoff * payoff;

        if (config_.use_antithetic) {
            // Use antithetic path (mirror random numbers)
            // This is simplified; proper implementation would generate antithetic path
            sum += payoff;  // Placeholder
        }
    }

    double actual_paths = config_.use_antithetic ? config_.num_paths : total_paths;
    double mean_payoff = sum / actual_paths;
    return std::exp(-r * T) * mean_payoff;
}

double MonteCarloModel::price_asian(const AsianOption& option, const MarketData& market_data) const {
    double S0 = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;

    double sum = 0.0;

    for (size_t i = 0; i < config_.num_paths; ++i) {
        auto path = simulate_path(S0, market_data, T, option.num_observations());
        double avg_price = option.calculate_average(path);
        double payoff = option.payoff(avg_price);
        sum += payoff;
    }

    double mean_payoff = sum / config_.num_paths;
    return std::exp(-r * T) * mean_payoff;
}

double MonteCarloModel::price_barrier(const BarrierOption& option, const MarketData& market_data) const {
    double S0 = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;

    double sum = 0.0;

    for (size_t i = 0; i < config_.num_paths; ++i) {
        auto path = simulate_path(S0, market_data, T, config_.num_timesteps);

        // Check if barrier was hit during the path
        bool barrier_hit = false;
        for (double S : path) {
            if (option.is_knocked(S)) {
                barrier_hit = true;
                break;
            }
        }

        double payoff;
        if (option.is_knock_in()) {
            // Knock-in: option active only if barrier is hit
            if (barrier_hit) {
                payoff = option.Option::payoff(path.back());  // Use base class payoff
            } else {
                payoff = option.rebate();
            }
        } else {
            // Knock-out: option inactive if barrier is hit
            if (barrier_hit) {
                payoff = option.rebate();
            } else {
                payoff = option.Option::payoff(path.back());
            }
        }

        sum += payoff;
    }

    double mean_payoff = sum / config_.num_paths;
    return std::exp(-r * T) * mean_payoff;
}

Greeks MonteCarloModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

} // namespace derivatives
