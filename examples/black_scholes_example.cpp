#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include "derivatives/products/digital_option.hpp"
#include <iostream>
#include <iomanip>

using namespace derivatives;

int main() {
    std::cout << "=== Black-Scholes Pricing Examples ===\n\n";
    std::cout << std::fixed << std::setprecision(4);

    // Define market data
    double spot = 100.0;
    double rate = 0.05;
    double volatility = 0.25;
    double dividend = 0.02;

    MarketData market(spot, rate, volatility, dividend);

    std::cout << "Market Data:\n";
    std::cout << "  Spot Price: $" << spot << "\n";
    std::cout << "  Risk-free Rate: " << rate * 100 << "%\n";
    std::cout << "  Volatility: " << volatility * 100 << "%\n";
    std::cout << "  Dividend Yield: " << dividend * 100 << "%\n\n";

    BlackScholesModel bs_model;

    // Example 1: ATM European Call
    std::cout << "Example 1: At-the-Money European Call\n";
    std::cout << "----------------------------------------\n";
    EuropeanOption atm_call(100.0, 1.0, OptionType::CALL);
    double call_price = bs_model.price(atm_call, market);
    Greeks call_greeks = bs_model.greeks(atm_call, market);

    std::cout << "Strike: $" << atm_call.strike() << "\n";
    std::cout << "Expiry: " << atm_call.expiry() << " years\n";
    std::cout << "Price: $" << call_price << "\n";
    std::cout << call_greeks << "\n\n";

    // Example 2: OTM European Put
    std::cout << "Example 2: Out-of-the-Money European Put\n";
    std::cout << "----------------------------------------\n";
    EuropeanOption otm_put(90.0, 0.5, OptionType::PUT);
    double put_price = bs_model.price(otm_put, market);
    Greeks put_greeks = bs_model.greeks(otm_put, market);

    std::cout << "Strike: $" << otm_put.strike() << "\n";
    std::cout << "Expiry: " << otm_put.expiry() << " years\n";
    std::cout << "Price: $" << put_price << "\n";
    std::cout << put_greeks << "\n\n";

    // Example 3: Implied Volatility
    std::cout << "Example 3: Implied Volatility Calculation\n";
    std::cout << "----------------------------------------\n";
    double market_price = 12.50;
    EuropeanOption option(105.0, 1.0, OptionType::CALL);

    MarketData guess_market(spot, rate, 0.3, dividend);  // Initial guess
    double impl_vol = bs_model.implied_volatility(market_price, option, guess_market);

    std::cout << "Market Price: $" << market_price << "\n";
    std::cout << "Strike: $" << option.strike() << "\n";
    std::cout << "Implied Volatility: " << impl_vol * 100 << "%\n\n";

    // Example 4: Digital (Binary) Option
    std::cout << "Example 4: Digital/Binary Option\n";
    std::cout << "----------------------------------------\n";
    DigitalOption digital_call(100.0, 1.0, OptionType::CALL, 100.0);
    BlackScholesDigitalModel digital_model;
    double digital_price = digital_model.price(digital_call, market);

    std::cout << "Payout if ITM: $" << digital_call.payout() << "\n";
    std::cout << "Digital Option Price: $" << digital_price << "\n\n";

    // Example 5: Option Price Sensitivity
    std::cout << "Example 5: Price vs Strike (Option Chain)\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Strike\tCall Price\tPut Price\tCall Delta\tPut Delta\n";

    for (double K = 90.0; K <= 110.0; K += 5.0) {
        EuropeanOption call(K, 1.0, OptionType::CALL);
        EuropeanOption put(K, 1.0, OptionType::PUT);

        double c_price = bs_model.price(call, market);
        double p_price = bs_model.price(put, market);
        Greeks c_greeks = bs_model.greeks(call, market);
        Greeks p_greeks = bs_model.greeks(put, market);

        std::cout << K << "\t" << c_price << "\t\t" << p_price
                  << "\t\t" << c_greeks.delta << "\t" << p_greeks.delta << "\n";
    }

    return 0;
}
