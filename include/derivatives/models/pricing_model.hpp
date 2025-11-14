#pragma once

#include "../core/greeks.hpp"
#include "../core/marketdata.hpp"
#include "../products/option.hpp"
#include <memory>

namespace derivatives {

// Abstract base class for all pricing models
class PricingModel {
public:
    virtual ~PricingModel() = default;

    // Price an option
    virtual double price(const Option& option, const MarketData& market_data) const = 0;

    // Calculate Greeks
    virtual Greeks greeks(const Option& option, const MarketData& market_data) const = 0;

protected:
    // Utility function to calculate Greeks numerically using finite differences
    Greeks numerical_greeks(const Option& option, const MarketData& market_data,
                           double spot_bump = 0.01, double vol_bump = 0.001,
                           double rate_bump = 0.0001, double time_bump = 1.0/365.0) const {
        Greeks g;

        double base_price = price(option, market_data);

        // Delta: dV/dS
        MarketData spot_up(market_data.spot * (1.0 + spot_bump), market_data.rate,
                          market_data.volatility, market_data.dividend);
        MarketData spot_down(market_data.spot * (1.0 - spot_bump), market_data.rate,
                            market_data.volatility, market_data.dividend);
        double price_up = price(option, spot_up);
        double price_down = price(option, spot_down);
        g.delta = (price_up - price_down) / (2.0 * market_data.spot * spot_bump);

        // Gamma: d²V/dS²
        g.gamma = (price_up - 2.0 * base_price + price_down) /
                  std::pow(market_data.spot * spot_bump, 2);

        // Vega: dV/dσ
        MarketData vol_up(market_data.spot, market_data.rate,
                         market_data.volatility + vol_bump, market_data.dividend);
        double vega_price = price(option, vol_up);
        g.vega = (vega_price - base_price) / vol_bump;

        // Theta: dV/dt (note: time to expiry decreases)
        Option theta_option(option.strike(), option.expiry() - time_bump,
                           option.type(), option.style());
        double theta_price = price(theta_option, market_data);
        g.theta = (theta_price - base_price) / time_bump;

        // Rho: dV/dr
        MarketData rate_up(market_data.spot, market_data.rate + rate_bump,
                          market_data.volatility, market_data.dividend);
        double rho_price = price(option, rate_up);
        g.rho = (rho_price - base_price) / rate_bump;

        return g;
    }
};

} // namespace derivatives
