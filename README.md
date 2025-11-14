# Derivatives Pricing Library

A comprehensive C++ library for pricing financial derivatives using various numerical methods and models.

## Features

### Pricing Models

- **Black-Scholes Model**: Analytical pricing for European options with Greeks calculation and implied volatility solver
- **Binomial Tree**: Cox-Ross-Rubinstein model for American and European options
- **Trinomial Tree**: Enhanced tree model for improved convergence
- **Monte Carlo Simulation**: Flexible MC engine with variance reduction techniques
- **LSMC (Longstaff-Schwartz)**: American option pricing using least-squares Monte Carlo
- **Heston Model**: Stochastic volatility model with semi-analytical and MC pricing

### Derivative Products

- **Vanilla Options**: European and American calls and puts
- **Asian Options**: Arithmetic and geometric averaging options
- **Barrier Options**: Up-and-in, up-and-out, down-and-in, down-and-out
- **Digital (Binary) Options**: Cash-or-nothing and asset-or-nothing options

### Mathematical Utilities

- **Random Number Generation**: Mersenne Twister with normal and uniform distributions
- **Statistical Functions**: Normal CDF/PDF, inverse CDF, mean, variance, correlation
- **Linear Algebra**: Matrix operations, linear solvers, LU and Cholesky decomposition
- **Interpolation**: Linear and cubic spline interpolation

### Market Data Structures

- **Yield Curves**: Flat, interpolated, and Nelson-Siegel models
- **Volatility Surfaces**: Flat, interpolated, local volatility (Dupire), and SABR models

## Directory Structure

```
Derivatives-Pricing-Library/
├── include/derivatives/
│   ├── core/           # Core types and data structures
│   ├── math/           # Mathematical utilities
│   ├── models/         # Pricing models
│   ├── products/       # Derivative products
│   └── curves/         # Yield curves and volatility surfaces
├── src/                # Source implementations
├── tests/              # Unit tests
├── examples/           # Example programs
└── CMakeLists.txt      # Build configuration
```

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- Standard math library (libm)

## Building

### Clone and Build

```bash
git clone <repository-url>
cd Derivatives-Pricing-Library
mkdir build && cd build
cmake ..
make
```

### Build Options

```bash
cmake -DBUILD_SHARED_LIBS=ON ..     # Build shared library (default)
cmake -DBUILD_TESTS=ON ..           # Build test suite (default)
cmake -DBUILD_EXAMPLES=ON ..        # Build examples (default)
cmake -DCMAKE_BUILD_TYPE=Release .. # Release build (default)
```

### Installation

```bash
sudo make install
```

## Usage

### Basic Example: Black-Scholes Pricing

```cpp
#include "derivatives/models/black_scholes.hpp"
#include "derivatives/products/option.hpp"

using namespace derivatives;

int main() {
    // Market data
    MarketData market(100.0,  // spot price
                     0.05,   // risk-free rate
                     0.25,   // volatility
                     0.0);   // dividend yield

    // European call option
    EuropeanOption call(100.0,  // strike
                       1.0,    // expiry (years)
                       OptionType::CALL);

    // Price the option
    BlackScholesModel model;
    double price = model.price(call, market);
    Greeks greeks = model.greeks(call, market);

    std::cout << "Option Price: $" << price << std::endl;
    std::cout << "Delta: " << greeks.delta << std::endl;

    return 0;
}
```

### American Option Pricing

```cpp
#include "derivatives/models/binomial_tree.hpp"

// American put option
AmericanOption put(100.0, 1.0, OptionType::PUT);

// Price using binomial tree
BinomialTreeModel tree(200);  // 200 steps
double price = tree.price(put, market);
```

### Asian Option Pricing

```cpp
#include "derivatives/models/monte_carlo.hpp"
#include "derivatives/products/asian_option.hpp"

// Asian call with arithmetic averaging
AsianOption asian(100.0, 1.0, OptionType::CALL,
                 AveragingType::ARITHMETIC, 12);

// Configure Monte Carlo
MonteCarloConfig config;
config.num_paths = 100000;
config.use_antithetic = true;

MonteCarloModel mc(config);
double price = mc.price_asian(asian, market);
```

### Barrier Option Pricing

```cpp
#include "derivatives/products/barrier_option.hpp"

// Up-and-out barrier call
BarrierOption barrier(100.0, 1.0, OptionType::CALL,
                     BarrierType::UP_AND_OUT,
                     120.0,  // barrier level
                     0.0);   // rebate

MonteCarloModel mc(config);
double price = mc.price_barrier(barrier, market);
```

### Volatility Surface

```cpp
#include "derivatives/curves/volatility_surface.hpp"

// SABR volatility model
SABRVolatilitySurface::SABRParams params;
params.alpha = 0.25;
params.beta = 0.5;
params.rho = -0.4;
params.nu = 0.3;

SABRVolatilitySurface surface(100.0, params);
double vol = surface.volatility(105.0, 1.0);  // strike, maturity
```

## Running Tests

```bash
cd build
ctest
```

Or run individual tests:

```bash
./tests/test_black_scholes
./tests/test_binomial_tree
./tests/test_monte_carlo
./tests/test_options
```

## Running Examples

```bash
./examples/black_scholes_example
./examples/american_option_example
./examples/asian_option_example
./examples/barrier_option_example
./examples/heston_example
./examples/volatility_surface_example
```

## API Reference

### Core Classes

#### MarketData
Holds market data required for pricing:
- `spot`: Underlying asset price
- `rate`: Risk-free interest rate
- `volatility`: Volatility (σ)
- `dividend`: Dividend yield

#### Option
Base class for all options:
- `strike()`: Strike price
- `expiry()`: Time to expiry
- `type()`: CALL or PUT
- `style()`: EUROPEAN, AMERICAN, or BERMUDAN
- `payoff(spot)`: Calculate payoff at given spot price

#### Greeks
Structure holding option sensitivities:
- `delta`: ∂V/∂S
- `gamma`: ∂²V/∂S²
- `vega`: ∂V/∂σ
- `theta`: ∂V/∂t
- `rho`: ∂V/∂r

### Pricing Models

All pricing models inherit from `PricingModel` and implement:
- `price(option, market_data)`: Calculate option price
- `greeks(option, market_data)`: Calculate Greeks

#### BlackScholesModel
- Analytical pricing for European options
- Fast and accurate
- Greeks calculation
- Implied volatility solver

#### BinomialTreeModel
- Supports American and European options
- Configurable number of steps
- Convergence to Black-Scholes for European options

#### MonteCarloModel
- Flexible configuration
- Variance reduction techniques
- Supports exotic options (Asian, Barrier)

#### LSMCModel
- American option pricing
- Longstaff-Schwartz algorithm
- Configurable basis functions

#### HestonModel
- Stochastic volatility
- Semi-analytical pricing
- Monte Carlo simulation

## Performance Considerations

- **Black-Scholes**: Fastest for European options (analytical)
- **Binomial Tree**: Good balance of speed and accuracy, use 100-500 steps
- **Monte Carlo**: Slower but flexible, use 50k-100k paths
- **LSMC**: Most expensive, use for American options when needed

## Compilation

To use the library in your project:

```cmake
find_package(DerivativesPricing REQUIRED)
target_link_libraries(your_target DerivativesPricing::derivatives_pricing)
```

Or manually:

```bash
g++ -std=c++17 -I/path/to/include your_code.cpp -L/path/to/lib -lderivatives_pricing -lm
```

## Testing

The library includes comprehensive tests covering:
- Option payoff calculations
- Pricing model accuracy
- Put-call parity
- Convergence properties
- Greeks calculation
- Exotic option features

## Examples

See the `examples/` directory for complete working examples:
- `black_scholes_example.cpp`: Basic vanilla option pricing
- `american_option_example.cpp`: American options with various models
- `asian_option_example.cpp`: Path-dependent options
- `barrier_option_example.cpp`: Barrier options with different types
- `heston_example.cpp`: Stochastic volatility modeling
- `volatility_surface_example.cpp`: Vol surface and yield curve usage

## Contributing

Contributions are welcome! Areas for enhancement:
- Additional exotic options (lookback, chooser, rainbow)
- Interest rate derivatives (swaps, caps, floors, swaptions)
- More stochastic models (SABR dynamics, jump-diffusion)
- Finite difference methods for PDEs
- Parallel Monte Carlo implementation
- GPU acceleration

## License

This library is provided as-is for educational and research purposes.

## References

- Hull, J. C. (2018). *Options, Futures, and Other Derivatives*
- Shreve, S. E. (2004). *Stochastic Calculus for Finance*
- Longstaff, F. A., & Schwartz, E. S. (2001). Valuing American Options by Simulation
- Heston, S. L. (1993). A Closed-Form Solution for Options with Stochastic Volatility

## Author

Created as a comprehensive derivatives pricing library for quantitative finance applications.
