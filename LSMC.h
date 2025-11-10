#include "PricingMain.h"

class LSMC : public PricingModel{

public:
    struct Config {
        size_t num_paths;
        size_t num_timesteps;
        unsigned int seed;
        bool use_antithetic;
        int polynomial_degree ;


        Config():num_paths(50000) , num_timesteps(50) , seed(12345) , use_antithetic(true) , polynomial_degree(3){}


    };

};
    