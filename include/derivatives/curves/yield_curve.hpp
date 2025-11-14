#pragma once

#include "../math/interpolation.hpp"
#include <vector>
#include <memory>
#include <map>

namespace derivatives {

// Abstract base class for yield curves
class YieldCurve {
public:
    virtual ~YieldCurve() = default;

    // Get zero rate for a given maturity
    virtual double zero_rate(double T) const = 0;

    // Get discount factor for a given maturity
    virtual double discount_factor(double T) const {
        return std::exp(-zero_rate(T) * T);
    }

    // Get forward rate between two times
    virtual double forward_rate(double T1, double T2) const {
        if (T2 <= T1) return zero_rate(T1);
        double df1 = discount_factor(T1);
        double df2 = discount_factor(T2);
        return -std::log(df2 / df1) / (T2 - T1);
    }
};

// Flat yield curve (constant rate)
class FlatYieldCurve : public YieldCurve {
public:
    explicit FlatYieldCurve(double rate) : rate_(rate) {}

    double zero_rate(double T) const override {
        return rate_;
    }

private:
    double rate_;
};

// Interpolated yield curve
class InterpolatedYieldCurve : public YieldCurve {
public:
    InterpolatedYieldCurve(const std::vector<double>& maturities,
                          const std::vector<double>& rates)
        : maturities_(maturities), rates_(rates),
          interpolator_(std::make_unique<math::LinearInterpolation>(maturities, rates)) {}

    double zero_rate(double T) const override {
        return (*interpolator_)(T);
    }

private:
    std::vector<double> maturities_;
    std::vector<double> rates_;
    std::unique_ptr<math::LinearInterpolation> interpolator_;
};

// Nelson-Siegel yield curve model
class NelsonSiegelCurve : public YieldCurve {
public:
    NelsonSiegelCurve(double beta0, double beta1, double beta2, double lambda)
        : beta0_(beta0), beta1_(beta1), beta2_(beta2), lambda_(lambda) {}

    double zero_rate(double T) const override {
        if (T <= 0.0) return beta0_ + beta1_;
        double factor = (1.0 - std::exp(-lambda_ * T)) / (lambda_ * T);
        return beta0_ + beta1_ * factor + beta2_ * (factor - std::exp(-lambda_ * T));
    }

private:
    double beta0_, beta1_, beta2_, lambda_;
};

} // namespace derivatives
