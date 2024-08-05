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
																.def("preliminary_checks", &pyAthenaAutocallable::preliminary_checks);
	
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
												.def("getExitBarrier", [](AthenaResult a) {return a.exit_barrier;})
												.def("getKillBarrier", [](AthenaResult a) {return a.kill_barrier;})
												.def("getUnderlyingPath", [](AthenaResult a) {return a.underlying_path;})
												.def("getObsDates", [](AthenaResult a) {return a.observation_dates;})
												.def("getInceptionSpot", [](AthenaResult a) {return a.inception_spot;})
												.def("getMaturity", [](AthenaResult a) {return a.maturity;})
												.def("getTerminationStatus",  [](AthenaResult a) {return a.termination_status;});

	using base = ArithmeticBrownianModel;
	py::class_<base>(m, "ABM").def(py::init<float,
												float,
												float,
												float,
												float,
												uint16_t,
												std::vector<float> &,
												std::vector<float> &,
												std::vector<float> &>())
									.def("print", &pyABM::print)
									.def("generate_path", &pyABM::generate_path)
									.def("generate_stock_price", &pyABM::generate_stock_price)
									.def("write_csv", &pyABM::write_csv);
	// py::class_<pyAthenaAutocallable>(m, "pricepath").def("price_path", &pyAthenaAutocallable::price_path);
	// py::class_<pyAthenaAutocallable>(m, "AthenaAutocallable").def("check_terminations", &check_terminations);
}