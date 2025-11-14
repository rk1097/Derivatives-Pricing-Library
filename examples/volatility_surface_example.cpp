#include "derivatives/curves/volatility_surface.hpp"
#include "derivatives/curves/yield_curve.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace derivatives;

int main() {
    std::cout << "=== Volatility Surface Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    // Example 1: Flat Volatility Surface
    std::cout << "Example 1: Flat Volatility Surface\n";
    std::cout << "-----------------------------------\n";

    FlatVolatilitySurface flat_vol(0.25);

    std::cout << "Flat Volatility: 25%\n";
    std::cout << "Vol at (K=100, T=1.0): " << flat_vol.volatility(100.0, 1.0) * 100 << "%\n";
    std::cout << "Vol at (K=120, T=2.0): " << flat_vol.volatility(120.0, 2.0) * 100 << "%\n\n";

    // Example 2: Interpolated Volatility Surface
    std::cout << "Example 2: Interpolated Volatility Surface\n";
    std::cout << "------------------------------------------\n";

    std::vector<double> strikes = {90.0, 100.0, 110.0};
    std::vector<double> maturities = {0.5, 1.0, 2.0};

    // Create a volatility smile/surface
    std::vector<std::vector<double>> vols = {
        {0.28, 0.25, 0.27},  // T=0.5
        {0.26, 0.23, 0.25},  // T=1.0
        {0.24, 0.21, 0.23}   // T=2.0
    };

    InterpolatedVolatilitySurface vol_surface(strikes, maturities, vols);

    std::cout << "Volatility Grid:\n";
    std::cout << "Strike\\Maturity\t";
    for (double T : maturities) {
        std::cout << T << "y\t";
    }
    std::cout << "\n";

    for (size_t i = 0; i < strikes.size(); ++i) {
        std::cout << "$" << strikes[i] << "\t\t";
        for (size_t j = 0; j < maturities.size(); ++j) {
            std::cout << vols[j][i] * 100 << "%\t";
        }
        std::cout << "\n";
    }

    std::cout << "\nInterpolated volatilities:\n";
    std::cout << "Vol at (K=95, T=0.75): " << vol_surface.volatility(95.0, 0.75) * 100 << "%\n";
    std::cout << "Vol at (K=105, T=1.5): " << vol_surface.volatility(105.0, 1.5) * 100 << "%\n\n";

    // Example 3: SABR Volatility Model
    std::cout << "Example 3: SABR Volatility Model\n";
    std::cout << "--------------------------------\n";

    SABRVolatilitySurface::SABRParams sabr_params;
    sabr_params.alpha = 0.25;
    sabr_params.beta = 0.5;
    sabr_params.rho = -0.4;
    sabr_params.nu = 0.3;

    double forward = 100.0;
    SABRVolatilitySurface sabr_surface(forward, sabr_params);

    std::cout << "SABR Parameters:\n";
    std::cout << "  Forward: $" << forward << "\n";
    std::cout << "  Alpha: " << sabr_params.alpha << "\n";
    std::cout << "  Beta: " << sabr_params.beta << "\n";
    std::cout << "  Rho: " << sabr_params.rho << "\n";
    std::cout << "  Nu: " << sabr_params.nu << "\n\n";

    std::cout << "SABR Implied Volatility Smile (T=1.0):\n";
    std::cout << "Strike\t\tImplied Vol\n";

    for (double K = 80.0; K <= 120.0; K += 10.0) {
        double impl_vol = sabr_surface.volatility(K, 1.0);
        std::cout << "$" << K << "\t\t" << impl_vol * 100 << "%\n";
    }

    std::cout << "\n";

    // Example 4: Using Volatility Surface for Pricing
    std::cout << "Example 4: Option Pricing with Vol Surface\n";
    std::cout << "------------------------------------------\n";

    MarketData base_market(100.0, 0.05, 0.0, 0.0);
    BlackScholesModel bs;

    std::cout << "Strike\t\tVol\t\tPrice\n";

    for (double K = 90.0; K <= 110.0; K += 5.0) {
        double vol = vol_surface.volatility(K, 1.0);
        MarketData strike_market(100.0, 0.05, vol, 0.0);

        EuropeanOption call(K, 1.0, OptionType::CALL);
        double price = bs.price(call, strike_market);

        std::cout << "$" << K << "\t\t" << vol * 100 << "%\t\t$" << price << "\n";
    }

    std::cout << "\n";

    // Example 5: Yield Curve Examples
    std::cout << "Example 5: Yield Curve Models\n";
    std::cout << "------------------------------\n\n";

    // Flat yield curve
    std::cout << "Flat Yield Curve (5%):\n";
    FlatYieldCurve flat_curve(0.05);
    std::cout << "  1Y rate: " << flat_curve.zero_rate(1.0) * 100 << "%\n";
    std::cout << "  5Y rate: " << flat_curve.zero_rate(5.0) * 100 << "%\n";
    std::cout << "  1Y discount factor: " << flat_curve.discount_factor(1.0) << "\n\n";

    // Interpolated yield curve
    std::cout << "Interpolated Yield Curve:\n";
    std::vector<double> curve_maturities = {1.0, 2.0, 5.0, 10.0};
    std::vector<double> curve_rates = {0.04, 0.045, 0.05, 0.052};

    InterpolatedYieldCurve interp_curve(curve_maturities, curve_rates);

    std::cout << "  Input rates:\n";
    for (size_t i = 0; i < curve_maturities.size(); ++i) {
        std::cout << "    " << curve_maturities[i] << "Y: " << curve_rates[i] * 100 << "%\n";
    }

    std::cout << "  Interpolated rates:\n";
    std::cout << "    3Y: " << interp_curve.zero_rate(3.0) * 100 << "%\n";
    std::cout << "    7Y: " << interp_curve.zero_rate(7.0) * 100 << "%\n";
    std::cout << "  Forward rate 1Y->2Y: " << interp_curve.forward_rate(1.0, 2.0) * 100 << "%\n\n";

    // Nelson-Siegel curve
    std::cout << "Nelson-Siegel Yield Curve:\n";
    NelsonSiegelCurve ns_curve(0.05, -0.02, 0.01, 1.0);

    std::cout << "  Maturity\tZero Rate\n";
    for (double T : {0.5, 1.0, 2.0, 5.0, 10.0}) {
        std::cout << "  " << T << "Y\t\t" << ns_curve.zero_rate(T) * 100 << "%\n";
    }

    return 0;
}
