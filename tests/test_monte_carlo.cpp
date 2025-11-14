#include "derivatives/models/monte_carlo.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include "derivatives/products/asian_option.hpp"
#include "derivatives/products/barrier_option.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace derivatives;

void test_european_option() {
    std::cout << "Testing Monte Carlo European Option Pricing...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    EuropeanOption call(100.0, 1.0, OptionType::CALL);

    MonteCarloConfig config;
    config.num_paths = 100000;
    config.num_timesteps = 100;
    config.use_antithetic = true;

    MonteCarloModel mc_model(config);
    double mc_price = mc_model.price(call, market);

    BlackScholesModel bs_model;
    double bs_price = bs_model.price(call, market);

    std::cout << "Monte Carlo Price: " << mc_price << "\n";
    std::cout << "Black-Scholes Price: " << bs_price << "\n";
    std::cout << "Difference: " << std::abs(mc_price - bs_price) << "\n";

    // Monte Carlo should be close to Black-Scholes (within 1% with 100k paths)
    double rel_error = std::abs(mc_price - bs_price) / bs_price;
    assert(rel_error < 0.02 && "MC should be close to BS");

    std::cout << "European option test PASSED!\n\n";
}

void test_asian_option() {
    std::cout << "Testing Monte Carlo Asian Option Pricing...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    AsianOption asian_call(100.0, 1.0, OptionType::CALL,
                          AveragingType::ARITHMETIC, 12);

    MonteCarloConfig config;
    config.num_paths = 50000;

    MonteCarloModel mc_model(config);
    double price = mc_model.price_asian(asian_call, market);

    std::cout << "Asian Call Option Price: " << price << "\n";

    // Asian option should be cheaper than vanilla due to averaging
    EuropeanOption vanilla_call(100.0, 1.0, OptionType::CALL);
    BlackScholesModel bs_model;
    double vanilla_price = bs_model.price(vanilla_call, market);

    std::cout << "Vanilla Call Price: " << vanilla_price << "\n";
    assert(price < vanilla_price && "Asian should be cheaper than vanilla");

    std::cout << "Asian option test PASSED!\n\n";
}

void test_barrier_option() {
    std::cout << "Testing Monte Carlo Barrier Option Pricing...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);

    // Up-and-out barrier option
    BarrierOption barrier_call(100.0, 1.0, OptionType::CALL,
                              BarrierType::UP_AND_OUT, 120.0, 0.0);

    MonteCarloConfig config;
    config.num_paths = 50000;
    config.num_timesteps = 252;  // Daily monitoring

    MonteCarloModel mc_model(config);
    double barrier_price = mc_model.price_barrier(barrier_call, market);

    std::cout << "Barrier Option Price: " << barrier_price << "\n";

    // Barrier option should be cheaper than vanilla
    EuropeanOption vanilla_call(100.0, 1.0, OptionType::CALL);
    BlackScholesModel bs_model;
    double vanilla_price = bs_model.price(vanilla_call, market);

    std::cout << "Vanilla Call Price: " << vanilla_price << "\n";
    assert(barrier_price < vanilla_price && "Barrier should be cheaper than vanilla");

    std::cout << "Barrier option test PASSED!\n\n";
}

int main() {
    std::cout << "=== Monte Carlo Model Tests ===\n\n";

    try {
        test_european_option();
        test_asian_option();
        test_barrier_option();

        std::cout << "=== ALL TESTS PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}
