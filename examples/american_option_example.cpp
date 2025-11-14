#include "derivatives/models/binomial_tree.hpp"
#include "derivatives/models/lsmc.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <iomanip>

using namespace derivatives;

int main() {
    std::cout << "=== American Option Pricing Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    // Market data
    MarketData market(100.0, 0.05, 0.25, 0.0);

    std::cout << "Market Data:\n";
    std::cout << "  Spot: $100, Rate: 5%, Volatility: 25%\n\n";

    // Example 1: American Put using Binomial Tree
    std::cout << "Example 1: American Put (Binomial Tree)\n";
    std::cout << "----------------------------------------\n";

    AmericanOption american_put(100.0, 1.0, OptionType::PUT);
    EuropeanOption european_put(100.0, 1.0, OptionType::PUT);

    std::cout << "Convergence with increasing steps:\n";
    std::cout << "Steps\tAmerican\tEuropean\tEarly Ex Premium\n";

    for (size_t steps : {50, 100, 200, 500}) {
        BinomialTreeModel tree(steps);
        double am_price = tree.price(american_put, market);
        double eu_price = tree.price(european_put, market);
        double premium = am_price - eu_price;

        std::cout << steps << "\t" << am_price << "\t\t" << eu_price
                  << "\t\t" << premium << "\n";
    }

    std::cout << "\n";

    // Example 2: American Put using LSMC
    std::cout << "Example 2: American Put (Longstaff-Schwartz Monte Carlo)\n";
    std::cout << "--------------------------------------------------------\n";

    LSMCModel::Config lsmc_config;
    lsmc_config.num_paths = 50000;
    lsmc_config.num_timesteps = 50;
    lsmc_config.polynomial_degree = 3;

    LSMCModel lsmc(lsmc_config);

    try {
        double lsmc_price = lsmc.price(american_put, market);
        std::cout << "LSMC American Put Price: $" << lsmc_price << "\n";
        std::cout << "Number of paths: " << lsmc_config.num_paths << "\n";
        std::cout << "Time steps: " << lsmc_config.num_timesteps << "\n\n";
    } catch (const std::exception& e) {
        std::cout << "LSMC pricing: " << e.what() << "\n\n";
    }

    // Example 3: Deep ITM American Put (Early Exercise)
    std::cout << "Example 3: Deep ITM American Put (Early Exercise Value)\n";
    std::cout << "-------------------------------------------------------\n";

    AmericanOption deep_itm_put(120.0, 1.0, OptionType::PUT);
    BinomialTreeModel tree(200);

    double deep_price = tree.price(deep_itm_put, market);
    double intrinsic = std::max(120.0 - 100.0, 0.0);
    double time_value = deep_price - intrinsic;

    std::cout << "Strike: $120 (Deep ITM)\n";
    std::cout << "American Put Price: $" << deep_price << "\n";
    std::cout << "Intrinsic Value: $" << intrinsic << "\n";
    std::cout << "Time Value: $" << time_value << "\n\n";

    // Example 4: American Call (No Dividends)
    std::cout << "Example 4: American Call with No Dividends\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "Note: American call = European call when no dividends\n\n";

    AmericanOption american_call(100.0, 1.0, OptionType::CALL);
    EuropeanOption european_call(100.0, 1.0, OptionType::CALL);

    double am_call_price = tree.price(american_call, market);
    double eu_call_price = tree.price(european_call, market);

    std::cout << "American Call Price: $" << am_call_price << "\n";
    std::cout << "European Call Price: $" << eu_call_price << "\n";
    std::cout << "Difference: $" << std::abs(am_call_price - eu_call_price) << "\n\n";

    // Example 5: Impact of Volatility on Early Exercise
    std::cout << "Example 5: Early Exercise Premium vs Volatility\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Volatility\tAmerican\tEuropean\tPremium\n";

    for (double vol = 0.1; vol <= 0.5; vol += 0.1) {
        MarketData vol_market(100.0, 0.05, vol, 0.0);
        AmericanOption am_put(110.0, 1.0, OptionType::PUT);
        EuropeanOption eu_put(110.0, 1.0, OptionType::PUT);

        BinomialTreeModel tree_model(200);
        double am_p = tree_model.price(am_put, vol_market);
        double eu_p = tree_model.price(eu_put, vol_market);

        std::cout << vol * 100 << "%\t\t" << am_p << "\t\t" << eu_p
                  << "\t\t" << (am_p - eu_p) << "\n";
    }

    return 0;
}
