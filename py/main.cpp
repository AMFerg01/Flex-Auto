#include "../cpp/include/flex_include/Autocallable.hpp"
#include "../cpp/include/flex_include/GeometricBrownianModel.hpp"
#include "../cpp/include/flex_include/debug_utilities.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;

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

	GeometricBrownianModel gbm = GeometricBrownianModel(drift, volatility, spot_price, maturity, step_size,
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

	// pyAthenaAutocallable athena_autocallable = pyAthenaAutocallable(
	// coupon_barrier,
 	// autocall_barrier,
    // exit_barrier,
    // kill_barrier,
	// autocall_value,
	// coupon_value,
	// kill_value,
    // maturity,
	// observation_dates
	// );

	athena_autocallable.price_path(gbm); 
	std::cout << athena_autocallable.autocall_value << std::endl;
}

class pyAthenaAutocallable : public AthenaAutocallable {
public:
	using AthenaAutocallable::AthenaAutocallable;
};

class pyGBM : public GeometricBrownianModel {
public:
	using GeometricBrownianModel::GeometricBrownianModel;
};



PYBIND11_MODULE(flex, m) {
    m.doc() = "python wrappers for Flex-Auto"; // optional module docstring
    m.def("run", &test_gbm, "Test");
	py::class_<pyAthenaAutocallable>(m, "pyAthenaAutocallable").def(py::init<float ,
																			float ,
																			float ,
																			float ,
																			float ,
																			float ,
																			float ,
																			float ,
																			std::vector<float> &>())
																.def("price_path", &pyAthenaAutocallable::price_path)
																.def("preliminary_checks", &pyAthenaAutocallable::preliminary_checks)
																.def("check_barrier_ordering", &pyAthenaAutocallable::check_barrier_ordering)
																.def("check_terminations", &pyAthenaAutocallable::check_terminations);
	
	py::class_<pyGBM>(m, "pyGBM").def(py::init<float,
												float,
												float,
												float,
												float,
												uint16_t,
												std::vector<float> &,
												std::vector<float> &,
												std::vector<float> &>())
									.def("print", &pyGBM::print)
									.def("print_path", &pyGBM::print_path)
									.def("print_stocks", &pyGBM::print_stocks)
									.def("generate_path", &pyGBM::generate_path)
									.def("compute_spot_prices", &pyGBM::compute_spot_prices);

	// py::class_<pyAthenaAutocallable>(m, "pricepath").def("price_path", &pyAthenaAutocallable::price_path);
	// py::class_<pyAthenaAutocallable>(m, "AthenaAutocallable").def("check_terminations", &check_terminations);
}