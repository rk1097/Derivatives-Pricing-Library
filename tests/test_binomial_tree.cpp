#include "derivatives/models/binomial_tree.hpp"
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace derivatives;

void test_european_convergence() {
    std::cout << "Testing Binomial Tree Convergence to Black-Scholes...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    EuropeanOption call(100.0, 1.0, OptionType::CALL);

    // Black-Scholes price (reference)
    BlackScholesModel bs_model;
    double bs_price = bs_model.price(call, market);

    std::cout << "Black-Scholes Price: " << bs_price << "\n";

    // Binomial tree with increasing steps
    for (size_t n_steps : {50, 100, 200, 500}) {
        BinomialTreeModel tree_model(n_steps);
        double tree_price = tree_model.price(call, market);
        double error = std::abs(tree_price - bs_price);

        std::cout << "Steps: " << n_steps
                  << ", Price: " << tree_price
                  << ", Error: " << error << "\n";
    }

    // With 500 steps, should be very close
    BinomialTreeModel tree_model(500);
    double tree_price = tree_model.price(call, market);
    assert(std::abs(tree_price - bs_price) < 0.1 && "Tree should converge to BS");

    std::cout << "European convergence test PASSED!\n\n";
}

void test_american_put() {
    std::cout << "Testing American Put Option...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    AmericanOption american_put(110.0, 1.0, OptionType::PUT);  // Deep ITM put
    EuropeanOption european_put(110.0, 1.0, OptionType::PUT);

    BinomialTreeModel tree_model(200);

    double american_price = tree_model.price(american_put, market);
    double european_price = tree_model.price(european_put, market);

    std::cout << "American Put Price: " << american_price << "\n";
    std::cout << "European Put Price: " << european_price << "\n";

    // American option should be worth at least as much as European
    assert(american_price >= european_price - 1e-6 &&
           "American option should be worth at least as much as European");

    // American option should be worth at least intrinsic value
    double intrinsic = std::max(110.0 - 100.0, 0.0);
    assert(american_price >= intrinsic - 1e-6 &&
           "American option should be worth at least intrinsic value");

    std::cout << "American put test PASSED!\n\n";
}

void test_trinomial_tree() {
    std::cout << "Testing Trinomial Tree Model...\n";

    MarketData market(100.0, 0.05, 0.2, 0.0);
    EuropeanOption call(100.0, 1.0, OptionType::CALL);

    TrinomialTreeModel tri_model(100);
    double tri_price = tri_model.price(call, market);

    BlackScholesModel bs_model;
    double bs_price = bs_model.price(call, market);

    std::cout << "Trinomial Tree Price: " << tri_price << "\n";
    std::cout << "Black-Scholes Price: " << bs_price << "\n";
    std::cout << "Difference: " << std::abs(tri_price - bs_price) << "\n";

    assert(std::abs(tri_price - bs_price) < 0.5 && "Trinomial should be close to BS");

    std::cout << "Trinomial tree test PASSED!\n\n";
}

int main() {
    std::cout << "=== Binomial/Trinomial Tree Tests ===\n\n";

    try {
        test_european_convergence();
        test_american_put();
        test_trinomial_tree();

        std::cout << "=== ALL TESTS PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
}
