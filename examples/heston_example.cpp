#include "derivatives/models/heston.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <iomanip>

using namespace derivatives;

int main() {
    std::cout << "=== Heston Stochastic Volatility Model Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    // Market data
    MarketData market(100.0, 0.05, 0.0, 0.0);  // Note: vol not used in Heston directly

    std::cout << "Market Data:\n";
    std::cout << "  Spot: $100, Rate: 5%\n\n";

    // Example 1: Heston with Different Parameter Sets
    std::cout << "Example 1: Heston Model with Different Parameters\n";
    std::cout << "------------------------------------------------\n\n";

    EuropeanOption call(100.0, 1.0, OptionType::CALL);

    // Low vol-of-vol (close to Black-Scholes)
    std::cout << "Low Vol-of-Vol (σ=0.1):\n";
    HestonParams params_low;
    params_low.kappa = 2.0;
    params_low.theta = 0.04;
    params_low.sigma = 0.1;  // Low vol-of-vol
    params_low.rho = -0.5;
    params_low.v0 = 0.04;

    HestonModel heston_low(params_low);

    try {
        double price_low = heston_low.price_monte_carlo(call, market, 50000, 100);
        std::cout << "  Call Price: $" << price_low << "\n";
        std::cout << "  kappa=" << params_low.kappa << ", theta=" << params_low.theta
                  << ", sigma=" << params_low.sigma << ", rho=" << params_low.rho << "\n\n";
    } catch (const std::exception& e) {
        std::cout << "  " << e.what() << "\n\n";
    }

    // High vol-of-vol
    std::cout << "High Vol-of-Vol (σ=0.5):\n";
    HestonParams params_high;
    params_high.kappa = 2.0;
    params_high.theta = 0.04;
    params_high.sigma = 0.5;  // High vol-of-vol
    params_high.rho = -0.7;
    params_high.v0 = 0.04;

    HestonModel heston_high(params_high);

    try {
        double price_high = heston_high.price_monte_carlo(call, market, 50000, 100);
        std::cout << "  Call Price: $" << price_high << "\n";
        std::cout << "  kappa=" << params_high.kappa << ", theta=" << params_high.theta
                  << ", sigma=" << params_high.sigma << ", rho=" << params_high.rho << "\n\n";
    } catch (const std::exception& e) {
        std::cout << "  " << e.what() << "\n\n";
    }

    // Example 2: Compare with Black-Scholes
    std::cout << "Example 2: Heston vs Black-Scholes\n";
    std::cout << "-----------------------------------\n";

    HestonParams params_bs_like;
    params_bs_like.kappa = 5.0;  // Fast mean reversion
    params_bs_like.theta = 0.0625;  // 25% vol
    params_bs_like.sigma = 0.0;  // No vol-of-vol (deterministic vol)
    params_bs_like.rho = 0.0;
    params_bs_like.v0 = 0.0625;

    HestonModel heston_bs_like(params_bs_like);

    MarketData bs_market(100.0, 0.05, 0.25, 0.0);
    BlackScholesModel bs;
    double bs_price = bs.price(call, bs_market);

    try {
        double heston_price = heston_bs_like.price_monte_carlo(call, market, 50000, 100);

        std::cout << "Black-Scholes Price: $" << bs_price << "\n";
        std::cout << "Heston Price (no vol-of-vol): $" << heston_price << "\n";
        std::cout << "Difference: $" << std::abs(bs_price - heston_price) << "\n\n";
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // Example 3: Impact of Correlation
    std::cout << "Example 3: Impact of Spot-Vol Correlation\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Correlation\tCall Price\n";

    for (double rho = -0.9; rho <= 0.0; rho += 0.3) {
        HestonParams params;
        params.kappa = 2.0;
        params.theta = 0.04;
        params.sigma = 0.3;
        params.rho = rho;
        params.v0 = 0.04;

        HestonModel heston(params);

        try {
            double price = heston.price_monte_carlo(call, market, 30000, 100);
            std::cout << std::setw(6) << rho << "\t\t" << price << "\n";
        } catch (const std::exception& e) {
            std::cout << std::setw(6) << rho << "\t\t" << "Error\n";
        }
    }

    std::cout << "\n";

    // Example 4: Volatility Smile
    std::cout << "Example 4: Volatility Smile from Heston\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Strike\t\tPrice\t\tImplied Vol\n";

    HestonParams smile_params;
    smile_params.kappa = 2.0;
    smile_params.theta = 0.04;
    smile_params.sigma = 0.4;
    smile_params.rho = -0.7;  // Negative correlation creates skew
    smile_params.v0 = 0.04;

    HestonModel heston_smile(smile_params);
    BlackScholesModel bs_iv;

    for (double K = 90.0; K <= 110.0; K += 5.0) {
        EuropeanOption opt(K, 1.0, OptionType::CALL);

        try {
            double heston_price = heston_smile.price_monte_carlo(opt, market, 30000, 100);

            // Calculate implied vol
            MarketData iv_market(100.0, 0.05, 0.25, 0.0);
            double impl_vol = 0.0;

            try {
                impl_vol = bs_iv.implied_volatility(heston_price, opt, iv_market);
                std::cout << "$" << K << "\t\t" << heston_price << "\t\t"
                          << (impl_vol * 100) << "%\n";
            } catch (...) {
                std::cout << "$" << K << "\t\t" << heston_price << "\t\tN/A\n";
            }
        } catch (const std::exception& e) {
            std::cout << "$" << K << "\t\tError: " << e.what() << "\n";
        }
    }

    return 0;
}
