#include "BrownianModel.hpp"
#include <cmath>
#include <cstdint>
#include <fstream>
#include <random>
#include <vector>

// register random variable from
// std rng.
std::random_device random_device{};
std::mt19937 generator{random_device()};
std::normal_distribution<float> standard_normal{0.0,1.0};


/* Arithmetic Brownian Model
Model for generating paths using Arithmetic brownian motion.
*/
ArithmeticBrownianModel::ArithmeticBrownianModel(
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
		this->termination_index = (uint)0; 
		this->path_to_termination = std::vector<float>({}); 

}

ArithmeticBrownianModel::~ArithmeticBrownianModel() {
};

/* print

Prints off the gbm experiment configuration.
*/
void ArithmeticBrownianModel::print(void) {
	  std::cout << "Arithmetic Brownian Model \n"
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
/* generate_path

Generates a path using random normal draws from the standard library.
*/
void ArithmeticBrownianModel::generate_path(void) {

	// declare iterators.
	auto wiener_iter = this->wiener_process.begin();
	auto paths_iter = this->paths.begin();

	*paths_iter = std::log(this->spot_price);
	paths_iter++;

	// generate the wiener process.
	for(; wiener_iter != this->wiener_process.end(); wiener_iter++)
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

/* generate_stock_price
Uses log-normal assumption to generate a path, then takes exponential.
*/
void ArithmeticBrownianModel::generate_stock_price(void) {
	generate_path();
	this->stocks = this->paths;
	for(auto p = stocks.begin(); p!= stocks.end(); p++)
		*p = expf(*p);
}


/* write_csv

Copied from the Ben Gorman's blog.
https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
, takes in a pair of string as filename,
vector of data columns.
*/
void ArithmeticBrownianModel::write_csv(std::string filename, std::vector<data_column> dataset)
{

	// Create an output filestream object
   std::ofstream my_file = std::ofstream(filename);

   // Send column names to the stream
   for(int j = 0; j < dataset.size(); ++j)
   {
       my_file << dataset.at(j).first;
       if(j != dataset.size() - 1) my_file << ","; // No comma at end of line
   }
   my_file << "\n";

   // Send data to the stream
   for(int i = 0; i < dataset.at(0).second.size(); ++i)
   {
       for(int j = 0; j < dataset.size(); ++j)
       {
           my_file << dataset.at(j).second.at(i);
           if(j != dataset.size() - 1) my_file << ","; // No comma at end of line
       }
       my_file << "\n";
   }

   // Close the file
   my_file.close();
}


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
		this->termination_index = (uint)0; 
		this->path_to_termination = std::vector<float>({}); 
}

GeometricBrownianModel::~GeometricBrownianModel() {
};


void GeometricBrownianModel::generate_path(void) {

	// declare iterators.
	auto wiener_iter = this->wiener_process.begin();
	auto paths_iter = this->paths.begin();

	*paths_iter = this->spot_price;
	paths_iter++;

	// generate the wiener process.
	for(; wiener_iter != this->wiener_process.end(); wiener_iter++)
	{
		*wiener_iter = standard_normal(generator);
	}
	wiener_iter = this->wiener_process.begin();


	float S_t, S_tp1, mu, W_t, sigma; 

	// generate brownian motion path.
	for(; paths_iter != this->paths.end(); paths_iter++) {

		// for readability only 
		S_t = *(paths_iter-1); 
		S_tp1 = *(paths_iter); 
		mu = drift; 
		sigma = volatility; 
		W_t = *wiener_iter;

		S_tp1 = S_t * expf( (drift *  - sigma * sigma * 0.5) * step_size + sigma * sqrtf(step_size) * W_t ); 

		// allocate results. 
		*paths_iter = S_tp1; 
		wiener_iter++; 

	}

}


void GeometricBrownianModel::generate_stock_price(void) {
	generate_path();
	this->stocks = this->paths;
}


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

  void GeometricBrownianModel::write_csv(std::string filename, std::vector<data_column> dataset)
{

	// Create an output filestream object
   std::ofstream my_file = std::ofstream(filename);

   // Send column names to the stream
   for(int j = 0; j < dataset.size(); ++j)
   {
       my_file << dataset.at(j).first;
       if(j != dataset.size() - 1) my_file << ","; // No comma at end of line
   }
   my_file << "\n";

   // Send data to the stream
   for(int i = 0; i < dataset.at(0).second.size(); ++i)
   {
       for(int j = 0; j < dataset.size(); ++j)
       {
           my_file << dataset.at(j).second.at(i);
           if(j != dataset.size() - 1) my_file << ","; // No comma at end of line
       }
       my_file << "\n";
   }

   // Close the file
   my_file.close();
}