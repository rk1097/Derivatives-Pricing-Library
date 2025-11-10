#pragma once
#include "OptionMain.h"


class PricingModel {

public:
    virtual ~PricingModel() = default;

    virtual double price(const Option& option, const MarketData& marketdata) const = 0;
    virtual Greeks greeks(const Option& option , const MarketData& marketdata) const = 0;

};