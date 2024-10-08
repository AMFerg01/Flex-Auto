#include "../cpp/include/flex_include/Autocallable.hpp"
#include "../cpp/include/flex_include/BrownianModel.hpp"
#include "../cpp/include/flex_include/debug_utilities.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;

class pyAthenaAutocallable : public AthenaAutocallable {
public:
	using AthenaAutocallable::AthenaAutocallable;
};

class pyABM : public ArithmeticBrownianModel {
public:
	using ArithmeticBrownianModel::ArithmeticBrownianModel;
};

class pyGBM : public GeometricBrownianModel {
public:
	using GeometricBrownianModel::GeometricBrownianModel;
};

class pyHBM : public HestonBrownianModel {
public:
	using HestonBrownianModel::HestonBrownianModel;
};

PYBIND11_MODULE(flexauto, m) {

	// register excpetion here. 	
	py::register_exception<BadAutocallParameterException>(m, "BadAutocallParameterException");
	py::register_exception<BadAutocallError>(m, "BadAutocallError");


    m.doc() = "python wrappers for Flex-Auto"; // optional module docstring
	py::class_<pyAthenaAutocallable>(m, "AthenaAutocallable").def(py::init<float,
																			float,
																			float,
																			float,
																			float,
																			float,
																			std::vector<float> &,
																			float,
																			float,
																			float>())
																.def("price_abm", &pyAthenaAutocallable::price_abm)
																.def("price_gbm", &pyAthenaAutocallable::price_gbm)
																.def("preliminary_checks", &pyAthenaAutocallable::preliminary_checks)
																.def("check_terminations", py::overload_cast<int, int, std::vector<float>,
																 											ArithmeticBrownianModel, bool>(&pyAthenaAutocallable::check_terminations))
																.def("check_terminations", py::overload_cast<int, int, std::vector<float>,
																 											GeometricBrownianModel, bool>(&pyAthenaAutocallable::check_terminations))
																.def("__repr__", [](const pyAthenaAutocallable & a ) {
																	return "AthenaAutocallable(exit=" + std::to_string(a.exit_barrier) + \
																	", autocall=" + std::to_string(a.autocall_barrier) + \
																	", coupon=" + std::to_string(a.coupon_barrier) + \
																	", kill=" + std::to_string(a.kill_barrier) +")";
																});
	
	
	py::class_<AthenaResult>(m, "AthenaResult").def(py::init<float,
															float,
															float,
															float,
															float,
															std::vector<float>&,
															float,
															float,
															float,
															float,
															std::vector<float> &>()) 
												.def("generate_json_dump", &AthenaResult::generate_json_dump)
												.def("getPrice", [](AthenaResult a) {return a.price;})
												.def("getAutocallBarrier", [](AthenaResult a) {return a.autocall_barrier;})
												.def("getACValue", [](AthenaResult a) {return a.autocall_value;})												
												.def("getExitBarrier", [](AthenaResult a) {return a.exit_barrier;})
												.def("getKillBarrier", [](AthenaResult a) {return a.kill_barrier;})
												.def("getKillValue", [](AthenaResult a) {return a.kill_value;})
												.def("getUnderlyingPath", [](AthenaResult a) {return a.underlying_path;})
												.def("getObsDates", [](AthenaResult a) {return a.observation_dates;})
												.def("getInceptionSpot", [](AthenaResult a) {return a.inception_spot;})
												.def("getMaturity", [](AthenaResult a) {return a.maturity;})
												.def("getTerminationStatus",  [](AthenaResult a) {return a.termination_status;})
												.def("__repr__", [](const AthenaResult &a) {
													return "AthenaResult(status=\"" + a.termination_status + "\", " + "price=" + std::to_string(a.price) + ")";
												});

	using base = ArithmeticBrownianModel;
	py::class_<base>(m, "ABM").def(py::init<float,
												float,
												float,
												float,
												float,
												uint16_t
												>())
									.def("print", &pyABM::print)
									.def("generate_path", &pyABM::generate_path)
									.def("generate_stock_price", &pyABM::generate_stock_price)
									.def("getStockPath", [](const ArithmeticBrownianModel abm) -> std::vector<float> { return abm.stocks; })
									.def("getTermIndex", [](const ArithmeticBrownianModel abm) -> uint { return abm.termination_index; })
									.def("getTermPath", [](const ArithmeticBrownianModel abm) -> std::vector<float> { return abm.path_to_termination; })
									.def("__repr__",
											[](const ArithmeticBrownianModel &abm) {
												return "ArithmeticBrownianModel(n=" + std::to_string(abm.number_of_steps) + ", " + "step_size=" + std::to_string(abm.step_size) +")";
											}
										)

									.def("write_csv", &pyABM::write_csv);


	using base_geo = GeometricBrownianModel;
	py::class_<base_geo>(m, "GBM").def(py::init<float,
												float,
												float,
												float,
												float,
												uint16_t
												>())
									.def("print", &pyGBM::print)
									.def("generate_path", &pyGBM::generate_path)
									.def("generate_stock_price", &pyGBM::generate_stock_price)
									.def("getStockPath", [](const GeometricBrownianModel gbm) -> std::vector<float> { return gbm.stocks; })
									.def("getTermIndex", [](const GeometricBrownianModel gbm) -> uint { return gbm.termination_index; })
									.def("getTermPath", [](const GeometricBrownianModel gbm) -> std::vector<float> { return gbm.path_to_termination; })
									.def("__repr__",
									[](const GeometricBrownianModel &gbm) {
										return "GeometricBrownianModel(n=" + std::to_string(gbm.number_of_steps) + ", " + "step_size=" + std::to_string(gbm.step_size) +")";
									}
									)
									.def("write_csv", &pyGBM::write_csv);

	using base_heston = HestonBrownianModel;
	py::class_<base_heston>(m, "HBM").def(py::init<float, // drift 
													float, //intial_volatility,
													float, //theta,
													float, //rho,
													float, //kappa,
													float, //ksi,
													float, //spot_price,
													float, //maturity, 
													float, //step_size,
													uint16_t //number_of_steps
												>())
									.def("print", &pyHBM::print)
									.def("generate_path", &pyHBM::generate_path)
									.def("generate_stock_price", &pyHBM::generate_stock_price)
									.def("getStockPath", [](const HestonBrownianModel hbm) -> std::vector<float> { return hbm.stocks; })
									.def("getTermIndex", [](const HestonBrownianModel hbm) -> uint { return hbm.termination_index; })
									.def("getTermPath", [](const HestonBrownianModel hbm) -> std::vector<float> { return hbm.path_to_termination; })
									.def("__repr__",
									[](const HestonBrownianModel &hbm) {
										return "HestonBrownianModel(n=" + std::to_string(hbm.number_of_steps) + ", " + "step_size=" + std::to_string(hbm.step_size) +")";
									}
									)
									.def("write_csv", &pyHBM::write_csv);

}