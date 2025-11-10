#pragma once 
#include <stdexcept>
#include <cmath>


struct MarketData{

    double spot_;
    double rate_;
    double volatility_;
    double dividend_;

    MarketData(double spot , double rate , double volatility , double dividend = 0.0) : spot_(spot) , rate_(rate) , volatility_(volatility_) , dividend_(dividend)
    {
        if (spot_ <= 0.0 || !std::isfinite(spot_)){
            throw std::invalid_argument("spot must be finite and positive");
        }
        if (volatility_ <= 0.0 || !std::isfinite(volatility_)){
            throw std::invalid_argument("volatility must be finite and positive");
        }
        if (!std::isfinite(rate_)){
            throw std::invalid_argument("rate must be finite");
        }
        if (!std::isfinite(dividend_)){
            throw std::invalid_argument("dividend must be finite");
        }
      
    }

};