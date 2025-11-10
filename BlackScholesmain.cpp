#include "BlackScholesmain.h"


double BlackScholes::calculate_d1(double S, double K, double T,double r, double q, double sigma) const {
    return (std::log(S/K) + (r-q+0.5*sigma*sigma)*T)/(sigma*std::sqrt(T));
}

double BlackScholes::calculate_d2(double d1 , double sigma , double T) const{
    return d1 - sigma*std::sqrt(T);
}

double BlackScholes::norm_cdf(double x) const {
    return 0.5*(1.0 + std::erf(x/std::sqrt(2.0)));
}

double BlackScholes::norm_pdf(double x) const {
    static const double inv_sqrt_2pi = 0.3989422804014327;

    return inv_sqrt_2pi*std::exp(-0.5*x*x);
}

double BlackScholes::price(const Option& option, const MarketData& marketdata) const
{
    double S = marketdata.spot_;
    double K = option.strike_;
    double T = option.expiry_;
    double sigma = marketdata.volatility_;
    double r = marketdata.rate_;
    double q = marketdata.dividend_;

    double d1  = calculate_d1(S,K,T,r,q,sigma);
    double d2 = calculate_d2(d1,sigma,T);

    if (option.type_ == Option::Type::CALL)
    {
        return S*std::exp(-q*T)*norm_cdf(d1) - K*std::exp(-r*T)*norm_cdf(d2);
    }
    else{
        return K*std::exp(-r*T)*norm_cdf(-d2) - S*std::exp(-q*T)*norm_cdf(-d1);
    }
}

Greeks BlackScholes::greeks(const Option& option, const MarketData& marketdata) const {
    double S = marketdata.spot_;
    double K = option.strike_;
    double T = option.expiry_;
    double sigma = marketdata.volatility_;
    double r = marketdata.rate_;
    double q = marketdata.dividend_;

    double d1  = calculate_d1(S,K,T,r,q,sigma);
    double d2 = calculate_d2(d1,sigma,T);
    double Nd1 = norm_cdf(d1);
    double Nd2 = norm_cdf(d2);
    double npd1 = norm_pdf(d1);
    double sqrt_T = std::sqrt(T);
    double exp_qT = std::exp(-q * T);
    double exp_rT = std::exp(-r * T);
    Greeks g;

    if (option.type_ == Option::Type::CALL)
    {
        g.delta = exp_qT * Nd1;
        g.gamma = exp_qT * npd1 / (S * sigma * sqrt_T);
        g.vega = S * exp_qT * npd1 * sqrt_T / 100.0;
        
        double theta_annual = -S * npd1 * sigma * exp_qT / (2.0 * sqrt_T)
                              - r * K * exp_rT * Nd2
                              + q * S * exp_qT * Nd1;
        g.theta = theta_annual / 365.0;
        
        g.rho = K * T * exp_rT * Nd2 / 100.0;
    }
    else{
        double N_minus_d1 = norm_cdf(-d1);
        double N_minus_d2 = norm_cdf(-d2);
        
        g.delta = exp_qT * (Nd1 - 1.0);
        g.gamma = exp_qT * npd1 / (S * sigma * sqrt_T);
        g.vega = S * exp_qT * npd1 * sqrt_T / 100.0;
        
        double theta_annual = -S * npd1 * sigma * exp_qT / (2.0 * sqrt_T)
                              + r * K * exp_rT * N_minus_d2
                              - q * S * exp_qT * N_minus_d1;
        g.theta = theta_annual / 365.0;
        
        g.rho = -K * T * exp_rT * N_minus_d2 / 100.0;
    }

    return g;
}