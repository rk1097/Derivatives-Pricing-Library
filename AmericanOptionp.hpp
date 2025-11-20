#pragma once 
#include "Optionp.hpp"
#include "EuropeanOptionp.hpp"
#include<cmath>


class AmericanOption : public Option {
public: 
    int num_steps_;
    AmericanOption(double spot , double strike , double rate , double T , double sigma , Optiontype type , int num_steps = 100) :
                                                                Option(spot , strike , rate , T , sigma,type),num_steps_(num_steps){}

    ~AmericanOption() = default;

    //virtual function in base class override

    double price() const override {
        // we are implementing binomial tree pricing for american option pricing
        double dt  = T_/ num_steps_;
        double u = std::exp(sigma_ * std::sqrt(dt));
        double d =  1.0/u;
        double p = ((std::exp(rate_*dt)) - d )/(u-d);

        // still needs to done

    }

    double delta() const override {
        double h = 0.01 *spot_;
        AmericanOption upoption(spot_+h , strike_ , rate_ , T_ , sigma_, type_,num_steps_);
        AmericanOption downoption(spot_-h , strike_ , rate_ , T_ , sigma_, type_,num_steps_);

        return (upoption.price() - downoption.price())/(2.0*h);
    }

    double gamma() const override {
        double h = 0.01 * spot_;
        AmericanOption upoption(spot_+h , strike_ , rate_ , T_ , sigma_, type_,num_steps_);
        AmericanOption downoption(spot_-h , strike_ , rate_ , T_ , sigma_, type_,num_steps_);
        return (upoption.price() + downoption.price()-2*price())/(h*h);
    }

    double theta() const override {
        double h = 1.0/365.0 ; 
        AmericanOption futureopt(spot_ , strike_ , rate_ , T_-h , sigma_, type_,num_steps_);
        return futureopt.price() - price();
    }
    double vega() const override {
        double h = 0.01; 
        AmericanOption upoptsigma(spot_ ,strike_, rate_, sigma_ + h, T_, type_, num_steps_);
        AmericanOption downoptsigma(spot_, strike_, rate_, sigma_ - h, T_, type_, num_steps_);
        return (upoptsigma.price() - downoptsigma.price())/2.0;

    }

    double earlyexcercisepremium(){
        EuropeanOption euroopt(spot_,strike_,rate_,T_,sigma_,type_);
        return price() - euroopt.price();
    }


};