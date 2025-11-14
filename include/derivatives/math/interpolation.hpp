#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace derivatives {
namespace math {

// Linear interpolation
class LinearInterpolation {
public:
    LinearInterpolation(const std::vector<double>& x, const std::vector<double>& y)
        : x_(x), y_(y) {
        if (x.size() != y.size() || x.size() < 2) {
            throw std::invalid_argument("Invalid interpolation data");
        }
    }

    double operator()(double x) const {
        if (x <= x_.front()) return y_.front();
        if (x >= x_.back()) return y_.back();

        auto it = std::lower_bound(x_.begin(), x_.end(), x);
        size_t i = std::distance(x_.begin(), it);
        if (i == 0) i = 1;

        double x0 = x_[i-1], x1 = x_[i];
        double y0 = y_[i-1], y1 = y_[i];

        return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
    }

private:
    std::vector<double> x_;
    std::vector<double> y_;
};

// Cubic spline interpolation
class CubicSplineInterpolation {
public:
    CubicSplineInterpolation(const std::vector<double>& x, const std::vector<double>& y);
    double operator()(double x) const;

private:
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> a_, b_, c_, d_;
    void compute_coefficients();
};

} // namespace math
} // namespace derivatives
