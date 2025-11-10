#include "BlackScholesmain.h"

bool approx_equal(double a , double b , double epsilon = 1e-6){
    return std::abs(a-b) < epsilon;
}

void test_call_pricing(){
    BlackScholes model;

    Option call(100.0,1.0,Option::Type::CALL);
    MarketData market(100.0,0.05,0.2,0.0);

    double price = model.price(call,market);

}