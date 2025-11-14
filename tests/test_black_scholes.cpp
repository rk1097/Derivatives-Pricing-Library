#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace derivatives;

void test_call_option() {
    std::cout << "Testing Black-Scholes Call Option Pricing...\n";

    // Market data
    MarketData market(100.0, 0.05, 0.2, 0.0);  // S=100, r=5%, vol=20%

    // European call option
    EuropeanOption call(100.0, 1.0, OptionType::CALL);  // K=100, T=1 year

    BlackScholesModel bs_model;
    double price = bs_model.price(call, market);
    Greeks greeks = bs_model.greeks(call, market);

    std::cout << "Call Option Price: " << price << "\n";
    std::cout << greeks << "\n";

    // Sanity checks
    assert(price > 0.0 && "Call price should be positive");
    assert(greeks.delta > 0.0 && greeks.delta < 1.0 && "Call delta should be in (0,1)");
    assert(greeks.gamma > 0.0 && "Gamma should be positive");
    assert(greeks.vega > 0.0 && "Vega should be positive");

    std::cout << "Call option test PASSED!\n\n";
}

void test_put_option() {
    std::cout << "Testing Black-Scholes Put Option Pricing...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    EuropeanOption put(100.0, 1.0, OptionType::PUT);

    BlackScholesModel bs_model;
    double price = bs_model.price(put, market);
    Greeks greeks = bs_model.greeks(put, market);

    std::cout << "Put Option Price: " << price << "\n";
    std::cout << greeks << "\n";

    assert(price > 0.0 && "Put price should be positive");
    assert(greeks.delta < 0.0 && greeks.delta > -1.0 && "Put delta should be in (-1,0)");

    std::cout << "Put option test PASSED!\n\n";
}

void test_put_call_parity() {
    std::cout << "Testing Put-Call Parity...\n";

    double S = 100.0, K = 100.0, T = 1.0, r = 0.05, vol = 0.2;
    MarketData market(S, r, vol, 0.0);

    EuropeanOption call(K, T, OptionType::CALL);
    EuropeanOption put(K, T, OptionType::PUT);

    BlackScholesModel bs_model;
    double call_price = bs_model.price(call, market);
    double put_price = bs_model.price(put, market);

    // Put-Call Parity: C - P = S - K*e^(-rT)
    double lhs = call_price - put_price;
    double rhs = S - K * std::exp(-r * T);

    std::cout << "Call Price: " << call_price << "\n";
    std::cout << "Put Price: " << put_price << "\n";
    std::cout << "LHS (C-P): " << lhs << "\n";
    std::cout << "RHS (S-Ke^-rT): " << rhs << "\n";
    std::cout << "Difference: " << std::abs(lhs - rhs) << "\n";

    assert(std::abs(lhs - rhs) < 1e-6 && "Put-Call Parity should hold");

    std::cout << "Put-Call Parity test PASSED!\n\n";
}

void test_implied_volatility() {
    std::cout << "Testing Implied Volatility Calculation...\n";

    double true_vol = 0.25;
    MarketData market(100.0, 0.05, true_vol, 0.0);
    EuropeanOption call(100.0, 1.0, OptionType::CALL);

    BlackScholesModel bs_model;
    double market_price = bs_model.price(call, market);

    // Now try to recover the volatility
    MarketData market_guess(100.0, 0.05, 0.3, 0.0);  // Start with different vol
    double implied_vol = bs_model.implied_volatility(market_price, call, market_guess);

    std::cout << "True Volatility: " << true_vol << "\n";
    std::cout << "Implied Volatility: " << implied_vol << "\n";
    std::cout << "Difference: " << std::abs(implied_vol - true_vol) << "\n";

    assert(std::abs(implied_vol - true_vol) < 1e-4 && "Implied vol should match true vol");

    std::cout << "Implied Volatility test PASSED!\n\n";
}

int main() {
    std::cout << "=== Black-Scholes Model Tests ===\n\n";

    try {
        test_call_option();
        test_put_option();
        test_put_call_parity();
        test_implied_volatility();

        std::cout << "=== ALL TESTS PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}
