#include "../../include/derivatives/models/lsmc.hpp"
#include "../../include/derivatives/math/matrix.hpp"
#include <cmath>
#include <algorithm>

namespace derivatives {

std::vector<std::vector<double>>
LSMCModel::generate_paths(double S0, const MarketData& market_data, double T) const {
    std::vector<std::vector<double>> paths(config_.num_paths,
                                           std::vector<double>(config_.num_timesteps + 1));

    double dt = T / config_.num_timesteps;
    double sigma = market_data.volatility;
    double r = market_data.rate;
    double q = market_data.dividend;
    double drift = (r - q - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);

    size_t actual_paths = config_.use_antithetic ? config_.num_paths / 2 : config_.num_paths;

    for (size_t i = 0; i < actual_paths; ++i) {
        paths[i][0] = S0;

        for (size_t j = 1; j <= config_.num_timesteps; ++j) {
            double dW = rng_.next_normal();
            paths[i][j] = paths[i][j - 1] * std::exp(drift + diffusion * dW);
        }

        // Antithetic paths
        if (config_.use_antithetic && i + actual_paths < config_.num_paths) {
            paths[i + actual_paths][0] = S0;
            for (size_t j = 1; j <= config_.num_timesteps; ++j) {
                // Reuse same path logic but with negated random numbers (simplified)
                paths[i + actual_paths][j] = paths[i][j];  // Placeholder
            }
        }
    }

    return paths;
}

std::vector<double> LSMCModel::basis_functions(double x, int degree) const {
    // Using Laguerre polynomials
    std::vector<double> basis(degree + 1);

    basis[0] = std::exp(-x / 2.0);
    if (degree >= 1) basis[1] = std::exp(-x / 2.0) * (1.0 - x);
    if (degree >= 2) basis[2] = std::exp(-x / 2.0) * (1.0 - 2.0 * x + x * x / 2.0);
    if (degree >= 3) basis[3] = std::exp(-x / 2.0) * (1.0 - 3.0 * x + 1.5 * x * x - x * x * x / 6.0);

    // For higher degrees, use recursion
    for (int i = 4; i <= degree; ++i) {
        basis[i] = ((2.0 * i - 1.0 - x) * basis[i - 1] - (i - 1.0) * basis[i - 2]) / i;
    }

    return basis;
}

std::vector<double> LSMCModel::regression(const std::vector<double>& x,
                                         const std::vector<double>& y,
                                         int degree) const {
    if (x.size() != y.size() || x.empty()) {
        throw std::invalid_argument("Invalid regression input");
    }

    size_t n = x.size();
    size_t m = degree + 1;

    // Build design matrix
    math::Matrix A(n, m);
    for (size_t i = 0; i < n; ++i) {
        auto basis = basis_functions(x[i], degree);
        for (size_t j = 0; j < m; ++j) {
            A(i, j) = basis[j];
        }
    }

    // Solve normal equations: A^T A beta = A^T y
    math::Matrix AT = A.transpose();
    math::Matrix ATA(m, m);

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k) {
                sum += A(k, i) * A(k, j);
            }
            ATA(i, j) = sum;
        }
    }

    std::vector<double> ATy(m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t k = 0; k < n; ++k) {
            ATy[i] += A(k, i) * y[k];
        }
    }

    return math::solve_linear_system(ATA, ATy);
}

double LSMCModel::evaluate_polynomial(const std::vector<double>& coeffs, double x) const {
    auto basis = basis_functions(x, coeffs.size() - 1);
    double result = 0.0;
    for (size_t i = 0; i < coeffs.size(); ++i) {
        result += coeffs[i] * basis[i];
    }
    return result;
}

double LSMCModel::price(const Option& option, const MarketData& market_data) const {
    if (option.style() != OptionStyle::AMERICAN) {
        throw std::invalid_argument("LSMC is designed for American options");
    }

    double S0 = market_data.spot;
    double T = option.expiry();
    double r = market_data.rate;
    double dt = T / config_.num_timesteps;

    // Generate all paths
    auto paths = generate_paths(S0, market_data, T);

    // Cash flow matrix (time to exercise and discounted value)
    std::vector<std::vector<double>> cash_flows(config_.num_paths,
                                                 std::vector<double>(config_.num_timesteps + 1, 0.0));

    // Initialize with terminal payoffs
    for (size_t i = 0; i < config_.num_paths; ++i) {
        cash_flows[i][config_.num_timesteps] = option.payoff(paths[i][config_.num_timesteps]);
    }

    // Backward induction
    for (int t = config_.num_timesteps - 1; t >= 1; --t) {
        // Collect in-the-money paths
        std::vector<double> x_itm, y_itm;
        std::vector<size_t> itm_indices;

        for (size_t i = 0; i < config_.num_paths; ++i) {
            double exercise_value = option.payoff(paths[i][t]);
            if (exercise_value > 0.0) {
                x_itm.push_back(paths[i][t]);

                // Discounted future cash flow
                double future_cf = 0.0;
                for (size_t j = t + 1; j <= config_.num_timesteps; ++j) {
                    if (cash_flows[i][j] > 0.0) {
                        future_cf = cash_flows[i][j] * std::exp(-r * dt * (j - t));
                        break;
                    }
                }
                y_itm.push_back(future_cf);
                itm_indices.push_back(i);
            }
        }

        // Perform regression if we have enough data points
        if (x_itm.size() >= static_cast<size_t>(config_.polynomial_degree + 1)) {
            auto coeffs = regression(x_itm, y_itm, config_.polynomial_degree);

            // Determine optimal exercise
            for (size_t k = 0; k < itm_indices.size(); ++k) {
                size_t i = itm_indices[k];
                double continuation_value = evaluate_polynomial(coeffs, paths[i][t]);
                double exercise_value = option.payoff(paths[i][t]);

                if (exercise_value > continuation_value) {
                    // Exercise now
                    cash_flows[i][t] = exercise_value;
                    // Clear future cash flows
                    for (size_t j = t + 1; j <= config_.num_timesteps; ++j) {
                        cash_flows[i][j] = 0.0;
                    }
                }
            }
        }
    }

    // Calculate option value as average of discounted cash flows
    double sum = 0.0;
    for (size_t i = 0; i < config_.num_paths; ++i) {
        for (size_t t = 0; t <= config_.num_timesteps; ++t) {
            if (cash_flows[i][t] > 0.0) {
                sum += cash_flows[i][t] * std::exp(-r * dt * t);
                break;
            }
        }
    }

    return sum / config_.num_paths;
}

Greeks LSMCModel::greeks(const Option& option, const MarketData& market_data) const {
    return numerical_greeks(option, market_data);
}

} // namespace derivatives
