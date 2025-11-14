#include "../../include/derivatives/curves/volatility_surface.hpp"
#include "../../include/derivatives/math/statistics.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace derivatives {

InterpolatedVolatilitySurface::InterpolatedVolatilitySurface(
    const std::vector<double>& strikes,
    const std::vector<double>& maturities,
    const std::vector<std::vector<double>>& volatilities)
    : strikes_(strikes), maturities_(maturities), volatilities_(volatilities) {

    if (volatilities.size() != maturities.size()) {
        throw std::invalid_argument("Volatility matrix size mismatch");
    }
    for (const auto& row : volatilities) {
        if (row.size() != strikes.size()) {
            throw std::invalid_argument("Volatility matrix size mismatch");
        }
    }
}

double InterpolatedVolatilitySurface::volatility(double strike, double maturity) const {
    return bilinear_interpolation(strike, maturity);
}

double InterpolatedVolatilitySurface::bilinear_interpolation(double strike, double maturity) const {
    // Find surrounding points
    auto k_it = std::lower_bound(strikes_.begin(), strikes_.end(), strike);
    auto t_it = std::lower_bound(maturities_.begin(), maturities_.end(), maturity);

    // Handle boundary cases
    if (strike <= strikes_.front()) k_it = strikes_.begin() + 1;
    if (strike >= strikes_.back()) k_it = strikes_.end() - 1;
    if (maturity <= maturities_.front()) t_it = maturities_.begin() + 1;
    if (maturity >= maturities_.back()) t_it = maturities_.end() - 1;

    size_t k1 = std::distance(strikes_.begin(), k_it) - 1;
    size_t k2 = k1 + 1;
    size_t t1 = std::distance(maturities_.begin(), t_it) - 1;
    size_t t2 = t1 + 1;

    double K1 = strikes_[k1], K2 = strikes_[k2];
    double T1 = maturities_[t1], T2 = maturities_[t2];

    double v11 = volatilities_[t1][k1];
    double v12 = volatilities_[t1][k2];
    double v21 = volatilities_[t2][k1];
    double v22 = volatilities_[t2][k2];

    // Bilinear interpolation
    double wK = (K2 - strike) / (K2 - K1);
    double wT = (T2 - maturity) / (T2 - T1);

    return wK * wT * v11 + (1.0 - wK) * wT * v12 +
           wK * (1.0 - wT) * v21 + (1.0 - wK) * (1.0 - wT) * v22;
}

LocalVolatilitySurface::LocalVolatilitySurface(const VolatilitySurface& implied_vol_surface,
                                               double spot, double rate, double dividend)
    : implied_vol_surface_(implied_vol_surface), spot_(spot), rate_(rate), dividend_(dividend) {}

double LocalVolatilitySurface::volatility(double strike, double maturity) const {
    return calculate_local_vol(strike, maturity);
}

double LocalVolatilitySurface::calculate_local_vol(double strike, double maturity) const {
    // Dupire formula (simplified numerical implementation)
    double dK = 0.01 * strike;
    double dT = 0.01;

    double sigma_impl = implied_vol_surface_.volatility(strike, maturity);

    // Numerical derivatives
    double dSigma_dK = (implied_vol_surface_.volatility(strike + dK, maturity) -
                       implied_vol_surface_.volatility(strike - dK, maturity)) / (2.0 * dK);

    double dSigma_dT = (implied_vol_surface_.volatility(strike, maturity + dT) -
                       implied_vol_surface_.volatility(strike, maturity)) / dT;

    double d2Sigma_dK2 = (implied_vol_surface_.volatility(strike + dK, maturity) -
                         2.0 * sigma_impl +
                         implied_vol_surface_.volatility(strike - dK, maturity)) / (dK * dK);

    // Dupire formula
    double numerator = sigma_impl * sigma_impl + 2.0 * sigma_impl * maturity *
                      (dSigma_dT + (rate_ - dividend_) * strike * dSigma_dK);

    double denominator = std::pow(1.0 + strike * dSigma_dK * std::sqrt(maturity), 2.0) +
                        sigma_impl * strike * strike * maturity * d2Sigma_dK2;

    if (denominator <= 0.0) return sigma_impl;  // Fallback

    double local_var = numerator / denominator;
    return local_var > 0.0 ? std::sqrt(local_var) : sigma_impl;
}

double SABRVolatilitySurface::volatility(double strike, double maturity) const {
    return sabr_implied_vol(strike, maturity);
}

double SABRVolatilitySurface::sabr_implied_vol(double strike, double maturity) const {
    // Hagan's SABR approximation formula
    double F = forward_;
    double K = strike;
    double alpha = params_.alpha;
    double beta = params_.beta;
    double rho = params_.rho;
    double nu = params_.nu;
    double T = maturity;

    if (std::abs(F - K) < 1e-10) {
        // ATM formula
        double term1 = alpha / std::pow(F, 1.0 - beta);
        double term2 = 1.0 + ((1.0 - beta) * (1.0 - beta) / 24.0 * alpha * alpha / std::pow(F, 2.0 - 2.0 * beta) +
                              0.25 * rho * beta * nu * alpha / std::pow(F, 1.0 - beta) +
                              (2.0 - 3.0 * rho * rho) / 24.0 * nu * nu) * T;
        return term1 * term2;
    }

    double FK = F * K;
    double log_FK = std::log(F / K);
    double z = (nu / alpha) * std::pow(FK, (1.0 - beta) / 2.0) * log_FK;
    double x_z = std::log((std::sqrt(1.0 - 2.0 * rho * z + z * z) + z - rho) / (1.0 - rho));

    double numerator = alpha;
    double denominator = std::pow(FK, (1.0 - beta) / 2.0) *
                        (1.0 + std::pow((1.0 - beta) * log_FK, 2.0) / 24.0 +
                         std::pow((1.0 - beta) * log_FK, 4.0) / 1920.0);

    double factor1 = z / x_z;
    double factor2 = 1.0 + ((1.0 - beta) * (1.0 - beta) / 24.0 * alpha * alpha / std::pow(FK, 1.0 - beta) +
                           0.25 * rho * beta * nu * alpha / std::pow(FK, (1.0 - beta) / 2.0) +
                           (2.0 - 3.0 * rho * rho) / 24.0 * nu * nu) * T;

    return (numerator / denominator) * factor1 * factor2;
}

} // namespace derivatives
