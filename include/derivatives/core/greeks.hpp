#pragma once

#include <iostream>

namespace derivatives {

// Structure to hold option Greeks
struct Greeks {
    double delta;   // Rate of change of option value with respect to underlying price
    double gamma;   // Rate of change of delta with respect to underlying price
    double vega;    // Sensitivity to volatility
    double theta;   // Time decay
    double rho;     // Sensitivity to interest rate

    Greeks()
        : delta(0.0), gamma(0.0), vega(0.0), theta(0.0), rho(0.0) {}

    Greeks(double d, double g, double v, double t, double r)
        : delta(d), gamma(g), vega(v), theta(t), rho(r) {}

    // Print Greeks to output stream
    friend std::ostream& operator<<(std::ostream& os, const Greeks& g) {
        os << "Greeks:\n"
           << "  Delta: " << g.delta << "\n"
           << "  Gamma: " << g.gamma << "\n"
           << "  Vega:  " << g.vega << "\n"
           << "  Theta: " << g.theta << "\n"
           << "  Rho:   " << g.rho;
        return os;
    }
};

} // namespace derivatives
