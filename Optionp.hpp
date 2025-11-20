#pragma once
#include <iostream>

/*
 * - Base abstract class for all options 
 * - Pure Virtual functions so derived classes has to implement these functions
 * - Virtual Destructor for no Memory leaks
 * - 
 */
class Option {

public:
    enum class Optiontype {CALL,PUT};

protected:
// Every Option needs Spot , Strike , rate , T , sigma  and also its type
    double spot_;
    double strike_;
    double rate_;
    double T_;
    double sigma_;
    Optiontype type_;



public:

    //Constructor 
    Option(double spot , double strike , double rate , double T , double sigma , Optiontype type) : spot_(spot) , strike_(strike) ,
                                                                                 rate_(rate) , T_(T) , sigma_(sigma) , type_(type){}
    //Destructor needs to be virtual for proper cleanup
    //without this deleting derived class object through a base pointer leaks memory
    virtual ~Option() = default;

    //Every option needs to implement price , greeks and derived classes needs to implement these
    //Vtable lookup to get these functions so vptr points to vtable and the table  contains function pointers for these functions

    virtual double price() const = 0;
    virtual double delta() const = 0;
    virtual double gamma() const = 0;
    virtual double theta() const = 0;
    virtual double vega() const = 0;

    // Non - Virtual functions and no vtable look up

    void getinfo() const {
        std:: cout << "Spot price" << spot_ << std::endl;
        std:: cout << "Strike price" << strike_ << std::endl;
        std:: cout << "Rate" << rate_ << std::endl;
        std:: cout << "Time to expiry" << T_ << std::endl;
        std:: cout << "Sigma" << sigma_ << std::endl;
        std:: cout << "Price of this option" << price() << std::endl;
        std:: cout << "delta" << delta() << std::endl;
        std:: cout << "gamma" << gamma() << std::endl;
    }

    // getter functions are parameters are protected

    double getspot() const {return spot_;}
    double getstrike() const {return strike_;}
    double getrate() const {return rate_;}
    double getexpiry() const {return T_;}
    double getsigma() const {return sigma_;}

    // Every option will have an intrinsic value 

    double intrinsic_value() const {
        if (type_ == Optiontype::CALL){return std::max(spot_ - strike_, 0.0);}
        else {return std::max(strike_ - spot_ , 0.0);}
    }

    //check in the money :

    bool isatm() const {return intrinsic_value() > 0.0 ;}

    double moneyness() const {return spot_/strike_;}

};