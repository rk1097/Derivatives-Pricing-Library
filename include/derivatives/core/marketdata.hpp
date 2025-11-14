#pragma once

#include <stdexcept>
#include <cmath>

namespace derivatives {

// Market data required for option pricing
struct MarketData {
    double spot;         // Current underlying price
    double rate;         // Risk-free interest rate
    double volatility;   // Volatility (sigma)
    double dividend;     // Dividend yield

    MarketData(double s, double r, double vol, double div = 0.0)
        : spot(s), rate(r), volatility(vol), dividend(div) {
        validate();
    }

private:
    void validate() const {
        if (spot <= 0.0 || !std::isfinite(spot)) {
            throw std::invalid_argument("Spot price must be positive and finite");
        }
        if (volatility <= 0.0 || !std::isfinite(volatility)) {
            throw std::invalid_argument("Volatility must be positive and finite");
        }
        if (!std::isfinite(rate)) {
            throw std::invalid_argument("Interest rate must be finite");
        }
        if (!std::isfinite(dividend)) {
            throw std::invalid_argument("Dividend yield must be finite");
        }
    }
};

} // namespace derivatives
