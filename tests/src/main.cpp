/** Entrypoint for Autocallable pricing

Runs AthenaAutocallable 

*/
#include "GeometricBrownianModel.hpp"
#include "Autocallable.hpp"
#include <cstdint>
#include <iostream>
#include "debug_utilities.hpp"


/* Tests out the basic Geometric Brownian Model
simple print test.
*/
void test_gbm(void) {

	// Geometric Brownian Model Parameters

	// define experiment configuration.
	float riskfree_rate, drift, volatility, spot_price, maturity;
	uint16_t number_of_steps;
	riskfree_rate = 0.05;
	drift = 0.0 + riskfree_rate;
	volatility = 0.2;
	spot_price = 100.0;
	maturity = 5.0;
	number_of_steps = static_cast<uint16_t>(1000);

	// calculate step_size.
	float step_size =  maturity / number_of_steps;

	// define wiener_process
	std::vector<float> wiener_process, paths, stocks;
	wiener_process.assign(number_of_steps, 0.0);
	paths.assign(number_of_steps, 0.0);
	stocks.assign(number_of_steps, 0.0);

	ArithmeticBrownianModel gbm = ArithmeticBrownianModel(drift, volatility, spot_price, maturity, step_size,
		number_of_steps, wiener_process, paths, stocks);

	gbm.generate_path();
	gbm.compute_spot_prices(); 


	// Athena Autocallable Parameters. 

	float exit_barrier = 3.0;
	float autocall_barrier = 1.0;
	float coupon_barrier= 1.0; 	
	float kill_barrier = 0.5; 
	float autocall_value = 1.0;
	float coupon_value = 0.05;
	float kill_value = 1.0;
	std::vector<float> observation_dates = {1.0, 2.0, 3.0};
	
	// declare athena autocallable trade. 
	AthenaAutocallable athena_autocallable = AthenaAutocallable(
	coupon_barrier,
 	autocall_barrier,
    exit_barrier,
    kill_barrier,
	autocall_value,
	coupon_value,
	kill_value,
    maturity,
	observation_dates
	);

	athena_autocallable.price_path(gbm); 
}

int main(int argc, char* argv[]) {

	std::cout << "Running autocallable executable \n";

	test_gbm();
	return 0;
}
