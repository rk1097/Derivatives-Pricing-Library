#pragma once
#include<cmath>
#include "Optionp.hpp"


class EuropeanOption : public Option {

public:
    //constructor here but it should also call base class constructor 

    EuropeanOption(double spot , double strike , double rate , double T , double sigma , Optiontype type) : 
                                                                    Option(spot , strike , rate , T , sigma , type) {}

    // Destructor for no memory leaks 
    ~EuropeanOption() = default ;

    // Virtual functions in Base class implementation Override

    double price() const override {

        if (type_ == Optiontype::CALL){
            return spot_* norm_cdf(d1()) - strike_*std::exp(-rate_*T_)*norm_cdf(d2());
        }

        else{
            return strike_*std::exp(-rate_*T_)*norm_cdf(-d2()) - spot_*norm_cdf(-d1());
        }

    }

    double delta() const override {
        if (type_ == Optiontype::CALL){
            return norm_cdf(d1());
        }

        else{
            return norm_cdf(d1()) - 1;
        }
    }

    double gamma() const override {
            return norm_pdf(d1())/ (spot_* sigma_ * std::sqrt(T_));
    }

    double theta() const override {
        if (type_ == Optiontype::CALL) {
            double term1 = -(spot_ * norm_pdf(d1()) * sigma_) / (2.0 * std::sqrt(T_));
            double term2 = rate_ * strike_ * std::exp(-rate_ * T_) * norm_cdf(d2());
            return (term1 - term2) / 365.0;
        } else {
            double term1 = -(spot_* norm_pdf(d1()) * sigma_) / (2.0 * std::sqrt(T_));
            double term2 = rate_ * strike_ * std::exp(-rate_ * T_) * norm_cdf(-d2());
            return (term1 + term2) / 365.0; 
        }
    }
    
    double vega() const override {
        return spot_ * norm_pdf(d1()) * std::sqrt(T_) / 100.0;
    }

// non virtual functions
private:
    inline double d1() const {return (std::log(spot_/strike_) + (rate_ + sigma_*sigma_*0.5)*T_)/(sigma_*std::sqrt(T_));}

    inline double d2() const { return d1() - sigma_*std::sqrt(T_);}
};

// helper functions 

inline double norm_cdf(double x) {return 0.5 * std::erfc(-x / std::sqrt(2));}

inline double norm_pdf(double x) {return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);}

