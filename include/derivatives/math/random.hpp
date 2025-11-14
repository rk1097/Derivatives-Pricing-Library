#pragma once

#include <random>
#include <vector>
#include <cmath>
#include <memory>

namespace derivatives {
namespace math {

// Random number generator for Monte Carlo simulations
class RandomGenerator {
public:
    explicit RandomGenerator(unsigned int seed = 12345)
        : generator_(seed), normal_dist_(0.0, 1.0), uniform_dist_(0.0, 1.0) {}

    // Generate standard normal random variable
    double next_normal() {
        return normal_dist_(generator_);
    }

    // Generate uniform random variable [0, 1]
    double next_uniform() {
        return uniform_dist_(generator_);
    }

    // Generate vector of normal random variables
    std::vector<double> normal_vector(size_t n) {
        std::vector<double> result(n);
        for (size_t i = 0; i < n; ++i) {
            result[i] = next_normal();
        }
        return result;
    }

    // Generate correlated normal random variables
    std::pair<double, double> correlated_normals(double correlation) {
        double z1 = next_normal();
        double z2 = next_normal();
        double w1 = z1;
        double w2 = correlation * z1 + std::sqrt(1.0 - correlation * correlation) * z2;
        return {w1, w2};
    }

    // Reset seed
    void set_seed(unsigned int seed) {
        generator_.seed(seed);
    }

private:
    std::mt19937 generator_;
    std::normal_distribution<double> normal_dist_;
    std::uniform_real_distribution<double> uniform_dist_;
};

// Box-Muller transform for generating normal random variables
inline std::pair<double, double> box_muller(double u1, double u2) {
    double r = std::sqrt(-2.0 * std::log(u1));
    double theta = 2.0 * M_PI * u2;
    return {r * std::cos(theta), r * std::sin(theta)};
}

// Sobol sequence generator for low-discrepancy sequences
class SobolGenerator {
public:
    SobolGenerator(size_t dimension = 1);
    std::vector<double> next();
    void reset();

private:
    size_t dimension_;
    size_t count_;
    static const size_t max_dimension = 40;
    static const size_t max_degree = 18;
};

} // namespace math
} // namespace derivatives
