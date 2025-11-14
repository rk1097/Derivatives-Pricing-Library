#pragma once

#include <vector>
#include <map>
#include <memory>
#include <cmath>

namespace derivatives {

// Abstract base class for volatility surfaces
class VolatilitySurface {
public:
    virtual ~VolatilitySurface() = default;

    // Get volatility for given strike and maturity
    virtual double volatility(double strike, double maturity) const = 0;

    // Get volatility for given moneyness and maturity
    virtual double volatility_by_moneyness(double moneyness, double maturity) const {
        return volatility(moneyness, maturity);  // Can be overridden
    }
};

// Flat volatility surface (constant volatility)
class FlatVolatilitySurface : public VolatilitySurface {
public:
    explicit FlatVolatilitySurface(double vol) : volatility_(vol) {}

    double volatility(double strike, double maturity) const override {
        return volatility_;
    }

private:
    double volatility_;
};

// Interpolated volatility surface using bilinear interpolation
class InterpolatedVolatilitySurface : public VolatilitySurface {
public:
    InterpolatedVolatilitySurface(const std::vector<double>& strikes,
                                 const std::vector<double>& maturities,
                                 const std::vector<std::vector<double>>& volatilities);

    double volatility(double strike, double maturity) const override;

private:
    std::vector<double> strikes_;
    std::vector<double> maturities_;
    std::vector<std::vector<double>> volatilities_;

    double bilinear_interpolation(double strike, double maturity) const;
};

// Local volatility surface (Dupire model)
class LocalVolatilitySurface : public VolatilitySurface {
public:
    LocalVolatilitySurface(const VolatilitySurface& implied_vol_surface,
                          double spot, double rate, double dividend);

    double volatility(double strike, double maturity) const override;

private:
    const VolatilitySurface& implied_vol_surface_;
    double spot_;
    double rate_;
    double dividend_;

    // Calculate local volatility from implied volatility
    double calculate_local_vol(double strike, double maturity) const;
};

// SABR volatility model
class SABRVolatilitySurface : public VolatilitySurface {
public:
    struct SABRParams {
        double alpha;   // Initial volatility
        double beta;    // CEV exponent
        double rho;     // Correlation
        double nu;      // Vol of vol

        SABRParams() : alpha(0.3), beta(0.5), rho(-0.3), nu(0.4) {}
        SABRParams(double a, double b, double r, double n)
            : alpha(a), beta(b), rho(r), nu(n) {}
    };

    SABRVolatilitySurface(double forward, const SABRParams& params)
        : forward_(forward), params_(params) {}

    double volatility(double strike, double maturity) const override;

private:
    double forward_;
    SABRParams params_;

    // SABR implied volatility formula (Hagan approximation)
    double sabr_implied_vol(double strike, double maturity) const;
};

} // namespace derivatives
