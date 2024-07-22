/** -- Autocallable.hpp
 * Author: Nik Pocuca
 * Date: April 12th 2024
 * Header file containing autocallable models for pricing options using monte-carlo based
 * simulations. Models currently supported are:
 * - AthenaAutocallable
 */

#include <vector>
#include "GeometricBrownianModel.hpp"

/** AthenaResult
 *
 * struct for holding information about the result of an AthenaAutocallable.
 *
 */
struct AthenaResult
{
    /* data */

    // barrier values for condition
    float coupon_barrier;
    float autocall_barrier;
    float exit_barrier;
    float kill_barrier;

    // reward values for condition met.
    float autocall_value;
    float coupon_value;
    float kill_value;

    float maturity;
    std::vector<float> observation_dates;

    // underlying specifics.
    float inception_spot;
    std::vector<float> underlying_path;
    GeometricBrownianModel gbm;

    float price;
    std::string termination_status;
    unsigned int terminating_index;
};

/** -- AthenaAutocallable

A class defining an autocallable for calculating a price from
paths, coupons can be earned on a snowball effect basis.

By definition autocall_barrier == coupon_barrier, we designate this autocallable
as athena.
*/
class AthenaAutocallable
{

public:
    float coupon_barrier;
    float autocall_barrier;
    float autocall_value;
    float coupon_value;
    float kill_value;
    float exit_barrier;
    float kill_barrier;
    float maturity;

    std::vector<float> observation_dates;

    // utility methods.
    void preliminary_checks(void);
    void check_barrier_ordering(void);

    // price methods
    AthenaAutocallable(float coupon_barrier,
                       float autocall_barrier,
                       float exit_barrier,
                       float kill_barrier,
                       float autocall_value,
                       float coupon_value,
                       float kill_value,
                       float maturity,
                       std::vector<float> & observation_dates);

    ~AthenaAutocallable();

    // TODO(remove this later.)
    void price_path(GeometricBrownianModel &gbm);

    AthenaResult price_path_d(GeometricBrownianModel &gbm);

    AthenaResult check_terminations(unsigned int i,
                                    unsigned int index,
                                    float spot_normalized,
                                    GeometricBrownianModel &gbm,
                                    bool maturity);
};
