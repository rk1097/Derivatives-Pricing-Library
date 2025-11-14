#include "derivatives/models/monte_carlo.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/asian_option.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <iomanip>

using namespace derivatives;

int main() {
    std::cout << "=== Asian Option Pricing Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    MarketData market(100.0, 0.05, 0.25, 0.0);

    std::cout << "Market Data:\n";
    std::cout << "  Spot: $100, Rate: 5%, Volatility: 25%\n\n";

    // Example 1: Arithmetic Asian Call
    std::cout << "Example 1: Arithmetic Average Asian Call\n";
    std::cout << "----------------------------------------\n";

    AsianOption arithmetic_call(100.0, 1.0, OptionType::CALL,
                               AveragingType::ARITHMETIC, 12);

    MonteCarloConfig config;
    config.num_paths = 100000;
    config.num_timesteps = 12;
    config.use_antithetic = true;

    MonteCarloModel mc(config);
    double asian_price = mc.price_asian(arithmetic_call, market);

    // Compare with vanilla European
    EuropeanOption vanilla_call(100.0, 1.0, OptionType::CALL);
    BlackScholesModel bs;
    double vanilla_price = bs.price(vanilla_call, market);

    std::cout << "Asian Call Price: $" << asian_price << "\n";
    std::cout << "Vanilla Call Price: $" << vanilla_price << "\n";
    std::cout << "Discount: $" << (vanilla_price - asian_price) << "\n";
    std::cout << "Discount %: " << ((vanilla_price - asian_price) / vanilla_price * 100) << "%\n\n";

    // Example 2: Geometric vs Arithmetic Averaging
    std::cout << "Example 2: Geometric vs Arithmetic Averaging\n";
    std::cout << "--------------------------------------------\n";

    AsianOption geometric_call(100.0, 1.0, OptionType::CALL,
                              AveragingType::GEOMETRIC, 12);

    double geom_price = mc.price_asian(geometric_call, market);
    double arith_price = mc.price_asian(arithmetic_call, market);

    std::cout << "Arithmetic Average Call: $" << arith_price << "\n";
    std::cout << "Geometric Average Call: $" << geom_price << "\n";
    std::cout << "Note: Geometric average is always ≤ arithmetic average\n\n";

    // Example 3: Asian Put Options
    std::cout << "Example 3: Asian Put Options\n";
    std::cout << "----------------------------\n";

    AsianOption arithmetic_put(100.0, 1.0, OptionType::PUT,
                              AveragingType::ARITHMETIC, 12);

    double asian_put_price = mc.price_asian(arithmetic_put, market);

    std::cout << "Asian Put Price: $" << asian_put_price << "\n\n";

    // Example 4: Impact of Number of Observations
    std::cout << "Example 4: Price vs Number of Averaging Points\n";
    std::cout << "----------------------------------------------\n";
    std::cout << "Observations\tPrice\t\tMC Error Estimate\n";

    for (size_t n_obs : {4, 12, 52, 252}) {
        AsianOption asian(100.0, 1.0, OptionType::CALL,
                         AveragingType::ARITHMETIC, n_obs);

        config.num_timesteps = n_obs;
        MonteCarloModel mc_temp(config);

        // Run multiple times to estimate MC error
        double sum = 0.0, sum_sq = 0.0;
        int n_runs = 5;

        for (int i = 0; i < n_runs; ++i) {
            config.seed = 12345 + i;
            MonteCarloModel mc_run(config);
            double price = mc_run.price_asian(asian, market);
            sum += price;
            sum_sq += price * price;
        }

        double mean = sum / n_runs;
        double variance = (sum_sq - sum * sum / n_runs) / (n_runs - 1);
        double std_error = std::sqrt(variance);

        std::cout << n_obs << "\t\t" << mean << "\t\t" << std_error << "\n";
    }

    std::cout << "\n";

    // Example 5: Asian Options at Different Strikes
    std::cout << "Example 5: Asian Option Chain\n";
    std::cout << "-----------------------------\n";
    std::cout << "Strike\t\tCall Price\tPut Price\n";

    for (double K = 90.0; K <= 110.0; K += 5.0) {
        AsianOption call(K, 1.0, OptionType::CALL, AveragingType::ARITHMETIC, 12);
        AsianOption put(K, 1.0, OptionType::PUT, AveragingType::ARITHMETIC, 12);

        config.seed = 12345;
        MonteCarloModel mc_chain(config);

        double call_price = mc_chain.price_asian(call, market);
        double put_price = mc_chain.price_asian(put, market);

        std::cout << K << "\t\t" << call_price << "\t\t" << put_price << "\n";
    }

    return 0;
}
