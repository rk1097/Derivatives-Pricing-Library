#include "../../include/derivatives/math/statistics.hpp"
#include <cmath>
#include <algorithm>

namespace derivatives {
namespace math {

// Beasley-Springer-Moro algorithm for inverse normal CDF
double norm_inv_cdf(double p) {
    if (p <= 0.0 || p >= 1.0) {
        throw std::invalid_argument("Probability must be in (0, 1)");
    }

    static const double a0 = 2.50662823884;
    static const double a1 = -18.61500062529;
    static const double a2 = 41.39119773534;
    static const double a3 = -25.44106049637;

    static const double b0 = -8.47351093090;
    static const double b1 = 23.08336743743;
    static const double b2 = -21.06224101826;
    static const double b3 = 3.13082909833;

    static const double c0 = 0.3374754822726147;
    static const double c1 = 0.9761690190917186;
    static const double c2 = 0.1607979714918209;
    static const double c3 = 0.0276438810333863;
    static const double c4 = 0.0038405729373609;
    static const double c5 = 0.0003951896511919;
    static const double c6 = 0.0000321767881768;
    static const double c7 = 0.0000002888167364;
    static const double c8 = 0.0000003960315187;

    double y = p - 0.5;

    if (std::abs(y) < 0.42) {
        double r = y * y;
        double x = y * (((a3 * r + a2) * r + a1) * r + a0) /
                       ((((b3 * r + b2) * r + b1) * r + b0) * r + 1.0);
        return x;
    } else {
        double r;
        if (y > 0.0) {
            r = 1.0 - p;
        } else {
            r = p;
        }
        r = std::log(-std::log(r));
        double x = c0 + r * (c1 + r * (c2 + r * (c3 + r * (c4 + r * (c5 + r * (c6 + r * (c7 + r * c8)))))));
        if (y < 0.0) {
            x = -x;
        }
        return x;
    }
}

} // namespace math
} // namespace derivatives
