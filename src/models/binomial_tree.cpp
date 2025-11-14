#include "../../include/derivatives/models/binomial_tree.hpp"
#include <cmath>
#include <algorithm>

namespace derivatives {

BinomialTreeModel::TreeParams
BinomialTreeModel::calculate_parameters(const MarketData& market_data, double T) const {
    TreeParams params;
    params.dt = T / num_steps_;

    // Cox-Ross-Rubinstein (CRR) parameterization
    double sigma = market_data.volatility;
    double r = market_data.rate;
    double q = market_data.dividend;

    params.u = std::exp(sigma * std::sqrt(params.dt));
    params.d = 1.0 / params.u;
    params.df = std::exp(-r * params.dt);

    // Risk-neutral probability
    double growth_factor = std::exp((r - q) * params.dt);
    params.p = (growth_factor - params.d) / (params.u - params.d);

    // Check for valid probability
    if (params.p < 0.0 || params.p > 1.0) {
        throw std::runtime_error("Invalid binomial tree parameters");
    }

    return params;
}

double BinomialTreeModel::price(const Option& option, const MarketData& market_data) const {
    TreeParams params = calculate_parameters(market_data, option.expiry());

    if (option.style() == OptionStyle::AMERICAN) {
        return price_american(option, market_data, params);
    } else {
        return price_european(option, market_data, params);
    }
}

double BinomialTreeModel::price_european(const Option& option, const MarketData& market_data,
                                        const TreeParams& params) const {
    double S = market_data.spot;

    // Build tree of option values at maturity
    std::vector<double> values(num_steps_ + 1);

    for (size_t i = 0; i <= num_steps_; ++i) {
        double S_T = S * std::pow(params.u, num_steps_ - i) * std::pow(params.d, i);
        values[i] = option.payoff(S_T);
    }

    // Backward induction
    for (int step = num_steps_ - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            values[i] = params.df * (params.p * values[i] + (1.0 - params.p) * values[i + 1]);
        }
    }

    return values[0];
}

double BinomialTreeModel::price_american(const Option& option, const MarketData& market_data,
                                        const TreeParams& params) const {
    double S = market_data.spot;

    // Build tree of option values at maturity
    std::vector<double> values(num_steps_ + 1);
    std::vector<double> spot_prices(num_steps_ + 1);

    for (size_t i = 0; i <= num_steps_; ++i) {
        spot_prices[i] = S * std::pow(params.u, num_steps_ - i) * std::pow(params.d, i);
        values[i] = option.payoff(spot_prices[i]);
    }

    // Backward induction with early exercise check
    for (int step = num_steps_ - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            // Calculate spot price at this node
            double S_node = S * std::pow(params.u, step - i) * std::pow(params.d, i);

            // Continuation value
            double continuation = params.df * (params.p * values[i] + (1.0 - params.p) * values[i + 1]);

            // Exercise value
            double exercise = option.payoff(S_node);

            // American option: take max of continuation and exercise
            values[i] = std::max(continuation, exercise);
        }
    }

    return values[0];
}

Greeks BinomialTreeModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

// Trinomial tree implementation
double TrinomialTreeModel::price(const Option& option, const MarketData& market_data) const {
    double S = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;
    double q = market_data.dividend;
    double sigma = market_data.volatility;

    double dt = T / num_steps_;
    double dx = sigma * std::sqrt(3.0 * dt);
    double nu = r - q - 0.5 * sigma * sigma;

    // Branching probabilities
    double pu = 0.5 * ((sigma * sigma * dt + nu * nu * dt * dt) / (dx * dx) + nu * dt / dx);
    double pm = 1.0 - (sigma * sigma * dt + nu * nu * dt * dt) / (dx * dx);
    double pd = 0.5 * ((sigma * sigma * dt + nu * nu * dt * dt) / (dx * dx) - nu * dt / dx);

    double df = std::exp(-r * dt);

    // Build tree
    size_t n_nodes = 2 * num_steps_ + 1;
    std::vector<double> values(n_nodes);

    // Initialize terminal values
    for (size_t i = 0; i < n_nodes; ++i) {
        int j = i - num_steps_;  // index from -N to N
        double S_T = S * std::exp(j * dx);
        values[i] = option.payoff(S_T);
    }

    // Backward induction
    for (int step = num_steps_ - 1; step >= 0; --step) {
        size_t start = num_steps_ - step;
        size_t end = num_steps_ + step;

        for (size_t i = start; i <= end; ++i) {
            double S_node = S * std::exp((i - num_steps_) * dx);

            double continuation = df * (pu * values[i + 1] + pm * values[i] + pd * values[i - 1]);

            if (option.style() == OptionStyle::AMERICAN) {
                double exercise = option.payoff(S_node);
                values[i] = std::max(continuation, exercise);
            } else {
                values[i] = continuation;
            }
        }
    }

    return values[num_steps_];
}

Greeks TrinomialTreeModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

} // namespace derivatives
