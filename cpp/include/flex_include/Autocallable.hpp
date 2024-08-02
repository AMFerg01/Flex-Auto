#include <exception>
#include <vector>
#include "GeometricBrownianModel.hpp"

/* -- AthenaResult class
contains the resultant information from a pricing call for athena autocall.
*/
#pragma once
class AthenaResult {

public:
    // contract specific attributes.
    float coupon_barrier, autocall_barrier, exit_barrier, kill_barrier, maturity;
    std::vector<float> observation_dates;

    // value attributes.
    float autocall_value, coupon_value, kill_value;

    // underlying specific information.
    float inception_spot;
    std::vector<float> underlying_path;

    float price;

    /* constructor and deconstructor */
    AthenaResult(float coupon_barrier, float autocall_barrier, float exit_barrier, float kill_barrier, float maturity,
                     const std::vector<float>& observation_dates,
                     float autocall_value, float coupon_value, float kill_value,
                     float inception_spot, const std::vector<float>& underlying_path);
    ~AthenaResult();

    /* generates json file for dumping results from athena. */
    void generate_json_dump(void);
};

/* -- BadAutocallParameterException

An exception specific to parameters of the autocall, is thrown
when a trade defined within an AthenaAutocallable is ill-defined.
*/
#pragma once
class BadAutocallParameterException : std::exception {
	private:
		const char *error_message;

	public:
	  BadAutocallParameterException(const char *message) {
	    this->error_message = message;
	  }

	  const char *what() { return this->error_message; }
};

/* -- AthenaAutocallable

A class defining an autocallable for calculating a price from
paths, coupons can be earned on a snowball effect basis.

By definition autocall_barrier == coupon_barrier, we designate this autocallable
as athena.
*/
#pragma once
class AthenaAutocallable {

public:
	float coupon_barrier;
	float autocall_barrier;
	float autocall_value;
	float exit_barrier;
	float kill_barrier;
	float maturity;
	float coupon_value, kill_value, inception_spot;
	std::vector<float> observation_dates;

	AthenaAutocallable(float coupon_barrier, float autocall_barrier,
                     float autocall_value, float exit_barrier,
                     float kill_barrier, float maturity,
                     std::vector<float> & observation_dates,
                     float coupon_value, float kill_value,
                     float inception_spot);

	/**
	preliminary_checks

	Checks whether or not the parameters of the autocall trade are coherent
	and do not contain any spurious or redundant settings that would make
	the trade simplified. Function ensures that
	**/
	void preliminary_checks(void);

	AthenaResult price_gbm(GeometricBrownianModel & gbm);


};


/*

# contract specific attributes.

    # underlying specific
    gbm: GeometricBrownianModel

    # return specific
    price: float
    termination_status: str
    terminating_index: int


def check_terminations(
        self,
        i: int,
        index: int,
        stock_normalized: np.ndarray,
        gbm: GeometricBrownianModel,
        maturity: bool,
    ) -> Optional[AthenaResult]:
        """-- calculates the terminiations whether or not youve been paid off.

        at a specific index, use the normalized stock values to check termination schemes.
        """

        result = AthenaResult(
            coupon_barrier=self.coupon_barrier,
            autocall_barrier=self.autocall_barrier,
            exit_barrier=self.exit_barrier,
            kill_barrier=self.kill_barrier,
            maturity=self.maturity,
            observation_dates=self.observation_dates,
            autocall_value=self.autocall_value,
            coupon_value=self.coupon_value,
            kill_value=self.kill_value,
            price=inf,
            inception_spot=gbm.stocks[0],
            underlying_path=gbm.stocks,
            gbm=gbm,
            termination_status="error",
            terminating_index=index,
        )

        # check if autocall is triggered
        if (self.autocall_barrier <= stock_normalized[index]) and (
            stock_normalized[index] < self.exit_barrier
        ):

            price = gbm.stocks[0] * (
                self.autocall_value + self.coupon_value * (i + 1)
            )  # set price to autocall and number of coupons
            result.termination_status = f"ac-barrier-{i+1}"
            result.underlying_path = result.underlying_path[: result.terminating_index]
            result.price = price

        # check if exit is triggered
        if stock_normalized[index] >= self.exit_barrier:
            price = gbm.stocks[index] * (1.0 + self.coupon_value * (i + 1))
            result.termination_status = f"exit-barrier-{i+1}"
            result.underlying_path = result.underlying_path[: result.terminating_index]
            result.price = price

        # check if kill barrier is triggered
        if stock_normalized[index] <= self.kill_barrier:
            price = gbm.stocks[0] * self.kill_value
            result.termination_status = f"kill-barrier-{i+1}"
            result.underlying_path = result.underlying_path[: result.terminating_index]
            result.price = price

        # check if nothing happens
        if (self.kill_barrier < stock_normalized[index]) and (
            stock_normalized[index] < self.autocall_barrier
        ):
            if maturity:
                price = gbm.stocks[0]
                result.termination_status = "maturity"
                result.price = price

            else:
                return None

        if result.price == inf:
            raise NotImplementedError("User should not be here")
        else:
            return result
*/
