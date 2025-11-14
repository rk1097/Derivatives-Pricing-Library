#include "derivatives/products/option.hpp"
#include "derivatives/products/asian_option.hpp"
#include "derivatives/products/barrier_option.hpp"
#include "derivatives/products/digital_option.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace derivatives;

void test_option_payoffs() {
    std::cout << "Testing Option Payoffs...\n";

    EuropeanOption call(100.0, 1.0, OptionType::CALL);
    EuropeanOption put(100.0, 1.0, OptionType::PUT);

    // Call payoff
    assert(call.payoff(110.0) == 10.0 && "Call payoff incorrect");
    assert(call.payoff(90.0) == 0.0 && "Call payoff should be zero OTM");

    // Put payoff
    assert(put.payoff(90.0) == 10.0 && "Put payoff incorrect");
    assert(put.payoff(110.0) == 0.0 && "Put payoff should be zero OTM");

    std::cout << "Option payoff test PASSED!\n\n";
}

void test_asian_averaging() {
    std::cout << "Testing Asian Option Averaging...\n";

    AsianOption arithmetic_asian(100.0, 1.0, OptionType::CALL,
                                AveragingType::ARITHMETIC, 4);

    std::vector<double> prices = {100.0, 105.0, 110.0, 115.0};

    double arith_avg = arithmetic_asian.calculate_average(prices);
    assert(std::abs(arith_avg - 107.5) < 1e-10 && "Arithmetic average incorrect");

    AsianOption geometric_asian(100.0, 1.0, OptionType::CALL,
                               AveragingType::GEOMETRIC, 4);

    double geom_avg = geometric_asian.calculate_average(prices);
    double expected_geom = std::pow(100.0 * 105.0 * 110.0 * 115.0, 0.25);
    assert(std::abs(geom_avg - expected_geom) < 1e-6 && "Geometric average incorrect");

    std::cout << "Asian averaging test PASSED!\n\n";
}

void test_barrier_detection() {
    std::cout << "Testing Barrier Option Detection...\n";

    BarrierOption up_and_out(100.0, 1.0, OptionType::CALL,
                            BarrierType::UP_AND_OUT, 120.0);

    assert(!up_and_out.is_knocked(119.0) && "Should not be knocked at 119");
    assert(up_and_out.is_knocked(120.0) && "Should be knocked at 120");
    assert(up_and_out.is_knocked(125.0) && "Should be knocked at 125");
    assert(up_and_out.is_knock_out() && "Should be knock-out type");
    assert(!up_and_out.is_knock_in() && "Should not be knock-in type");

    BarrierOption down_and_in(100.0, 1.0, OptionType::PUT,
                             BarrierType::DOWN_AND_IN, 80.0);

    assert(!down_and_in.is_knocked(81.0) && "Should not be knocked at 81");
    assert(down_and_in.is_knocked(80.0) && "Should be knocked at 80");
    assert(down_and_in.is_knocked(75.0) && "Should be knocked at 75");
    assert(down_and_in.is_knock_in() && "Should be knock-in type");
    assert(!down_and_in.is_knock_out() && "Should not be knock-out type");

    std::cout << "Barrier detection test PASSED!\n\n";
}

void test_digital_payoffs() {
    std::cout << "Testing Digital Option Payoffs...\n";

    DigitalOption digital_call(100.0, 1.0, OptionType::CALL, 10.0);

    assert(digital_call.payoff(105.0) == 10.0 && "Digital call should pay 10 ITM");
    assert(digital_call.payoff(95.0) == 0.0 && "Digital call should pay 0 OTM");

    AssetOrNothingOption asset_call(100.0, 1.0, OptionType::CALL);

    assert(asset_call.payoff(110.0) == 110.0 && "Asset-or-nothing should pay spot ITM");
    assert(asset_call.payoff(90.0) == 0.0 && "Asset-or-nothing should pay 0 OTM");

    std::cout << "Digital payoff test PASSED!\n\n";
}

int main() {
    std::cout << "=== Option Product Tests ===\n\n";

    try {
        test_option_payoffs();
        test_asian_averaging();
        test_barrier_detection();
        test_digital_payoffs();

        std::cout << "=== ALL TESTS PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}
