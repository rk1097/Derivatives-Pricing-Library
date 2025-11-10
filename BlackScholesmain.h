#pragma once

# include "PricingMain.h"

class BlackScholes : public PricingModel{
public:
    BlackScholes() = default;
    double price(const Option& option, const MarketData& marketdata) const override;
    Greeks greeks(const Option& option, const MarketData& marketdata) const override;
private:
    double calculate_d1(double S, double K, double T,double r, double q, double sigma) const;
    double calculate_d2(double d1 , double sigma , double T) const;
    double norm_cdf(double x) const;
    double norm_pdf(double x) const;

};