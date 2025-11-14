#include "derivatives/models/monte_carlo.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/barrier_option.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <iomanip>

using namespace derivatives;

int main() {
    std::cout << "=== Barrier Option Pricing Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    MarketData market(100.0, 0.05, 0.25, 0.0);

    std::cout << "Market Data:\n";
    std::cout << "  Spot: $100, Rate: 5%, Volatility: 25%\n\n";

    MonteCarloConfig config;
    config.num_paths = 100000;
    config.num_timesteps = 252;  // Daily monitoring
    config.use_antithetic = true;

    MonteCarloModel mc(config);

    // Example 1: Up-and-Out Call
    std::cout << "Example 1: Up-and-Out Barrier Call\n";
    std::cout << "-----------------------------------\n";

    BarrierOption up_out_call(100.0, 1.0, OptionType::CALL,
                             BarrierType::UP_AND_OUT, 120.0, 0.0);

    double up_out_price = mc.price_barrier(up_out_call, market);

    // Compare with vanilla
    EuropeanOption vanilla_call(100.0, 1.0, OptionType::CALL);
    BlackScholesModel bs;
    double vanilla_price = bs.price(vanilla_call, market);

    std::cout << "Barrier Level: $120\n";
    std::cout << "Barrier Call Price: $" << up_out_price << "\n";
    std::cout << "Vanilla Call Price: $" << vanilla_price << "\n";
    std::cout << "Discount: $" << (vanilla_price - up_out_price) << "\n\n";

    // Example 2: Down-and-Out Put
    std::cout << "Example 2: Down-and-Out Barrier Put\n";
    std::cout << "------------------------------------\n";

    BarrierOption down_out_put(100.0, 1.0, OptionType::PUT,
                               BarrierType::DOWN_AND_OUT, 80.0, 0.0);

    double down_out_price = mc.price_barrier(down_out_put, market);

    std::cout << "Barrier Level: $80\n";
    std::cout << "Barrier Put Price: $" << down_out_price << "\n\n";

    // Example 3: Up-and-In Call
    std::cout << "Example 3: Up-and-In Barrier Call\n";
    std::cout << "----------------------------------\n";

    BarrierOption up_in_call(100.0, 1.0, OptionType::CALL,
                            BarrierType::UP_AND_IN, 120.0, 0.0);

    double up_in_price = mc.price_barrier(up_in_call, market);

    std::cout << "Barrier Level: $120\n";
    std::cout << "Up-and-In Call Price: $" << up_in_price << "\n";
    std::cout << "Up-and-Out Call Price: $" << up_out_price << "\n";
    std::cout << "Sum (should ≈ vanilla): $" << (up_in_price + up_out_price) << "\n";
    std::cout << "Vanilla Call Price: $" << vanilla_price << "\n\n";

    // Example 4: Barrier Options with Rebate
    std::cout << "Example 4: Barrier Option with Rebate\n";
    std::cout << "--------------------------------------\n";

    BarrierOption rebate_option(100.0, 1.0, OptionType::CALL,
                               BarrierType::UP_AND_OUT, 120.0, 5.0);

    double rebate_price = mc.price_barrier(rebate_option, market);

    std::cout << "Rebate Amount: $5\n";
    std::cout << "Option Price with Rebate: $" << rebate_price << "\n";
    std::cout << "Option Price without Rebate: $" << up_out_price << "\n";
    std::cout << "Rebate Value: $" << (rebate_price - up_out_price) << "\n\n";

    // Example 5: Impact of Barrier Level
    std::cout << "Example 5: Up-and-Out Call vs Barrier Level\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "Barrier\t\tPrice\t\tDiscount from Vanilla\n";

    for (double barrier = 110.0; barrier <= 150.0; barrier += 10.0) {
        BarrierOption barrier_call(100.0, 1.0, OptionType::CALL,
                                  BarrierType::UP_AND_OUT, barrier, 0.0);

        double price = mc.price_barrier(barrier_call, market);
        double discount = vanilla_price - price;

        std::cout << "$" << barrier << "\t\t" << price << "\t\t" << discount << "\n";
    }

    std::cout << "\n";

    // Example 6: Monitoring Frequency Impact
    std::cout << "Example 6: Impact of Monitoring Frequency\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Note: Continuous monitoring makes barrier options cheaper\n\n";
    std::cout << "Timesteps/Year\tPrice\n";

    BarrierOption test_barrier(100.0, 1.0, OptionType::CALL,
                              BarrierType::UP_AND_OUT, 120.0, 0.0);

    for (size_t steps : {12, 52, 252, 365}) {
        config.num_timesteps = steps;
        config.seed = 12345;
        MonteCarloModel mc_freq(config);

        double price = mc_freq.price_barrier(test_barrier, market);
        std::cout << steps << "\t\t" << price << "\n";
    }

    return 0;
}
