#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace derivatives {
namespace math {

// Cumulative distribution function for standard normal distribution
inline double norm_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

// Probability density function for standard normal distribution
inline double norm_pdf(double x) {
    static const double inv_sqrt_2pi = 0.3989422804014327;
    return inv_sqrt_2pi * std::exp(-0.5 * x * x);
}

// Inverse cumulative distribution function (quantile function)
// Using Beasley-Springer-Moro algorithm
double norm_inv_cdf(double p);

// Calculate mean of a vector
inline double mean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

// Calculate variance of a vector
inline double variance(const std::vector<double>& data) {
    if (data.size() <= 1) return 0.0;
    double m = mean(data);
    double sum_sq = 0.0;
    for (double x : data) {
        double diff = x - m;
        sum_sq += diff * diff;
    }
    return sum_sq / (data.size() - 1);
}

// Calculate standard deviation
inline double std_dev(const std::vector<double>& data) {
    return std::sqrt(variance(data));
}

// Calculate covariance between two vectors
inline double covariance(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() <= 1) {
        throw std::invalid_argument("Vectors must have same size > 1");
    }
    double mean_x = mean(x);
    double mean_y = mean(y);
    double sum = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        sum += (x[i] - mean_x) * (y[i] - mean_y);
    }
    return sum / (x.size() - 1);
}

// Calculate correlation coefficient
inline double correlation(const std::vector<double>& x, const std::vector<double>& y) {
    double cov = covariance(x, y);
    double std_x = std_dev(x);
    double std_y = std_dev(y);
    if (std_x == 0.0 || std_y == 0.0) return 0.0;
    return cov / (std_x * std_y);
}

// Calculate percentile
inline double percentile(std::vector<double> data, double p) {
    if (data.empty()) return 0.0;
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Percentile must be between 0 and 1");
    }
    std::sort(data.begin(), data.end());
    size_t index = static_cast<size_t>(p * (data.size() - 1));
    return data[index];
}

} // namespace math
} // namespace derivatives
