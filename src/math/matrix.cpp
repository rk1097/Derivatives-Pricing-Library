#include "../../include/derivatives/math/matrix.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace derivatives {
namespace math {

std::vector<double> solve_linear_system(Matrix A, std::vector<double> b) {
    size_t n = A.rows();
    if (A.cols() != n || b.size() != n) {
        throw std::invalid_argument("Matrix must be square and match vector size");
    }

    // Gaussian elimination with partial pivoting
    for (size_t i = 0; i < n; ++i) {
        // Find pivot
        size_t max_row = i;
        double max_val = std::abs(A(i, i));
        for (size_t k = i + 1; k < n; ++k) {
            if (std::abs(A(k, i)) > max_val) {
                max_val = std::abs(A(k, i));
                max_row = k;
            }
        }

        // Swap rows
        if (max_row != i) {
            for (size_t k = i; k < n; ++k) {
                std::swap(A(i, k), A(max_row, k));
            }
            std::swap(b[i], b[max_row]);
        }

        // Check for singular matrix
        if (std::abs(A(i, i)) < 1e-10) {
            throw std::runtime_error("Matrix is singular or nearly singular");
        }

        // Eliminate column
        for (size_t k = i + 1; k < n; ++k) {
            double factor = A(k, i) / A(i, i);
            for (size_t j = i; j < n; ++j) {
                A(k, j) -= factor * A(i, j);
            }
            b[k] -= factor * b[i];
        }
    }

    // Back substitution
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; ++j) {
            sum += A(i, j) * x[j];
        }
        x[i] = (b[i] - sum) / A(i, i);
    }

    return x;
}

std::pair<Matrix, Matrix> lu_decomposition(const Matrix& A) {
    size_t n = A.rows();
    if (A.cols() != n) {
        throw std::invalid_argument("Matrix must be square");
    }

    Matrix L(n, n, 0.0);
    Matrix U(n, n, 0.0);

    for (size_t i = 0; i < n; ++i) {
        // Upper triangular
        for (size_t k = i; k < n; ++k) {
            double sum = 0.0;
            for (size_t j = 0; j < i; ++j) {
                sum += L(i, j) * U(j, k);
            }
            U(i, k) = A(i, k) - sum;
        }

        // Lower triangular
        L(i, i) = 1.0;
        for (size_t k = i + 1; k < n; ++k) {
            double sum = 0.0;
            for (size_t j = 0; j < i; ++j) {
                sum += L(k, j) * U(j, i);
            }
            L(k, i) = (A(k, i) - sum) / U(i, i);
        }
    }

    return {L, U};
}

Matrix cholesky_decomposition(const Matrix& A) {
    size_t n = A.rows();
    if (A.cols() != n) {
        throw std::invalid_argument("Matrix must be square");
    }

    Matrix L(n, n, 0.0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < j; ++k) {
                sum += L(i, k) * L(j, k);
            }

            if (i == j) {
                double val = A(i, i) - sum;
                if (val <= 0.0) {
                    throw std::runtime_error("Matrix is not positive definite");
                }
                L(i, j) = std::sqrt(val);
            } else {
                L(i, j) = (A(i, j) - sum) / L(j, j);
            }
        }
    }

    return L;
}

} // namespace math
} // namespace derivatives
