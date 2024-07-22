#include "GeometricBrownianModel.hpp"
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

// register random variable from
// std rng.
std::random_device random_device{};
std::mt19937 generator{random_device()};
std::normal_distribution<float> standard_normal{0.0,1.0};


/* Geometric Brownian Model
Model for generating paths using geometric brownian motion.
*/
GeometricBrownianModel::GeometricBrownianModel(
	// asset related.
	float drift,
	float volatility,
	float spot_price,
	float maturity,
	float step_size,
	uint16_t number_of_steps,
	std::vector<float> & wiener_process,
	std::vector<float> & paths,
	std::vector<float> & stocks) {

		this->drift = drift;
		this->volatility = volatility;
		this->spot_price = spot_price;
		this->maturity = maturity;
		this->step_size = step_size;
		this->number_of_steps = number_of_steps;
		this->wiener_process = wiener_process;
		this->paths = paths;
		this->stocks = stocks;

}

GeometricBrownianModel::~GeometricBrownianModel() {
};

/* print

Prints off the gbm experiment configuration.
*/
void GeometricBrownianModel::print(void) {
	  std::cout << "Geometric Brownian Model \n"
	  << "{ \n"
	  << "drift: " << this->drift << "\n"
	  << "volatility: " << this->volatility << "\n"
	  << "spot_price: " << this->spot_price << "\n"
	  << "maturity: " << this->maturity << "\n"
	  << "step_size: " << this->step_size << "\n"
	  << "wiener_process: " << "size[" << this->wiener_process.size() << "]" << "capacity[" << this->wiener_process.capacity() << "]"  << "\n"
	  << "paths: " << "size[" << this->paths.size() << "]" << "capacity[" << this->paths.capacity() << "]" << "\n"
	  << "stocks: " << "size[" << this->stocks.size() << "]" << "capacity[" << this->stocks.capacity() << "]"  "\n"
	  << "} \n";
  }

/* print_path 

prints the number of steps equivalent to steps taken. 

*/
void GeometricBrownianModel::print_path(unsigned int num_steps){
	
	if (num_steps >= number_of_steps){
		throw std::runtime_error("Number of steps specified to print exceeds the number of steps for the path");
	}

	for(auto i=0; i < num_steps; i++ ){
		std::cout << this->paths.at(i) << "\n"; 
	}

}

void GeometricBrownianModel::print_stocks(unsigned int num_steps){

	if (num_steps >= number_of_steps){
		throw std::runtime_error("Number of steps specified to print exceeds the number of steps for the path");
	}

	for(auto i=0; i < num_steps; i++ ){
		std::cout << this->stocks.at(i) << "\n"; 
	}
}
/* generate_path

Generates a path using random normal draws from the standard library.
*/
void GeometricBrownianModel::generate_path(void) {

	// declare iterators.
	auto wiener_iter = this->wiener_process.begin();
	auto paths_iter = this->paths.begin();

	*paths_iter = std::log(this->spot_price);
	paths_iter++;

	// generate the wiener process.
	for(; wiener_iter != this->wiener_process.end() + 1; wiener_iter++)
	{
		*wiener_iter = standard_normal(generator);
	}
	wiener_iter = this->wiener_process.begin();

	// generate brownian motion path.
	for(; paths_iter != this->paths.end(); paths_iter++) {
		*paths_iter = (*(paths_iter-1)) + (drift - 0.5 * volatility * volatility) * step_size
			+ volatility * sqrtf(step_size) * (*wiener_iter);
		wiener_iter++;
	}

}


/** compute_spot_prices
 * 
 * Assumes a path is generated and you can compute the path accordingly. 
*/
void GeometricBrownianModel::compute_spot_prices(void) {

	for (auto i=0; i < paths.size(); i++){
		stocks.at(i) = expf(paths.at(i));
	}
	
}