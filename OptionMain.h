#pragma once
#include "MarketDatamain.h"


struct Option {
    double strike_;
    double expiry_;

    enum class Type {
        CALL,
        PUT
    };
    Type type_;

    Option(double strike , double expiry , Type type ) : strike_(strike) , expiry_(expiry) , type_(type){

        if (!std::isfinite(strike_) || strike_ < 0.0){
            throw std::invalid_argument("strike price must be positive and finite");
        }
        if (!std::isfinite(expiry_)|| expiry_ <= 0.0){
            throw std::invalid_argument("expiry must be positive and finite");
        }

    }

};

struct  Greeks{
    double delta;
    double gamma;
    double vega;
    double theta;
    double rho;

    Greeks() : delta(0) , gamma(0) , vega(0) , theta(0) , rho(0){

    }
};