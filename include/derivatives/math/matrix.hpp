#pragma once

#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace derivatives {
namespace math {

// Simple matrix class for numerical computations
class Matrix {
public:
    Matrix(size_t rows, size_t cols, double init_value = 0.0)
        : rows_(rows), cols_(cols), data_(rows * cols, init_value) {}

    // Access element
    double& operator()(size_t i, size_t j) {
        return data_[i * cols_ + j];
    }

    const double& operator()(size_t i, size_t j) const {
        return data_[i * cols_ + j];
    }

    // Getters
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    // Matrix-vector multiplication
    std::vector<double> multiply(const std::vector<double>& vec) const {
        if (vec.size() != cols_) {
            throw std::invalid_argument("Vector size must match matrix columns");
        }
        std::vector<double> result(rows_, 0.0);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                result[i] += (*this)(i, j) * vec[j];
            }
        }
        return result;
    }

    // Transpose
    Matrix transpose() const {
        Matrix result(cols_, rows_);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // Set row
    void set_row(size_t i, const std::vector<double>& row) {
        if (row.size() != cols_) {
            throw std::invalid_argument("Row size must match matrix columns");
        }
        for (size_t j = 0; j < cols_; ++j) {
            (*this)(i, j) = row[j];
        }
    }

    // Get row
    std::vector<double> get_row(size_t i) const {
        std::vector<double> row(cols_);
        for (size_t j = 0; j < cols_; ++j) {
            row[j] = (*this)(i, j);
        }
        return row;
    }

private:
    size_t rows_;
    size_t cols_;
    std::vector<double> data_;
};

// Solve linear system Ax = b using Gaussian elimination
std::vector<double> solve_linear_system(Matrix A, std::vector<double> b);

// Perform LU decomposition
std::pair<Matrix, Matrix> lu_decomposition(const Matrix& A);

// Cholesky decomposition for positive definite matrices
Matrix cholesky_decomposition(const Matrix& A);

} // namespace math
} // namespace derivatives
