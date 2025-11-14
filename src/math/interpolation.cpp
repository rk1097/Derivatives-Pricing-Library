#include "../../include/derivatives/math/interpolation.hpp"
#include "../../include/derivatives/math/matrix.hpp"
#include <algorithm>

namespace derivatives {
namespace math {

CubicSplineInterpolation::CubicSplineInterpolation(const std::vector<double>& x,
                                                   const std::vector<double>& y)
    : x_(x), y_(y) {
    if (x.size() != y.size() || x.size() < 2) {
        throw std::invalid_argument("Invalid interpolation data");
    }
    compute_coefficients();
}

void CubicSplineInterpolation::compute_coefficients() {
    size_t n = x_.size() - 1;
    a_.resize(n + 1);
    b_.resize(n);
    c_.resize(n + 1);
    d_.resize(n);

    for (size_t i = 0; i <= n; ++i) {
        a_[i] = y_[i];
    }

    // Natural spline boundary conditions: c[0] = c[n] = 0
    std::vector<double> h(n);
    for (size_t i = 0; i < n; ++i) {
        h[i] = x_[i + 1] - x_[i];
    }

    // Build tridiagonal system
    Matrix A(n + 1, n + 1, 0.0);
    std::vector<double> rhs(n + 1, 0.0);

    A(0, 0) = 1.0;
    A(n, n) = 1.0;

    for (size_t i = 1; i < n; ++i) {
        A(i, i - 1) = h[i - 1];
        A(i, i) = 2.0 * (h[i - 1] + h[i]);
        A(i, i + 1) = h[i];
        rhs[i] = 3.0 * ((a_[i + 1] - a_[i]) / h[i] - (a_[i] - a_[i - 1]) / h[i - 1]);
    }

    // Solve for c coefficients
    c_ = solve_linear_system(A, rhs);

    // Calculate b and d coefficients
    for (size_t i = 0; i < n; ++i) {
        b_[i] = (a_[i + 1] - a_[i]) / h[i] - h[i] * (2.0 * c_[i] + c_[i + 1]) / 3.0;
        d_[i] = (c_[i + 1] - c_[i]) / (3.0 * h[i]);
    }
}

double CubicSplineInterpolation::operator()(double x) const {
    if (x <= x_.front()) return y_.front();
    if (x >= x_.back()) return y_.back();

    auto it = std::lower_bound(x_.begin(), x_.end(), x);
    size_t i = std::distance(x_.begin(), it);
    if (i == 0) i = 1;
    --i;

    double dx = x - x_[i];
    return a_[i] + b_[i] * dx + c_[i] * dx * dx + d_[i] * dx * dx * dx;
}

} // namespace math
} // namespace derivatives
