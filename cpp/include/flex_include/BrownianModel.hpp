#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#define data_column std::pair<std::string, std::vector<float>>

#pragma once
class ArithmeticBrownianModel {
public:
  // asset related
  float drift, volatility, spot_price;

  // time related.
  float maturity, step_size;
  uint16_t number_of_steps;

  // indexed values.
  std::vector<float> wiener_process, paths, stocks;
  std::vector<float> path_to_termination; 
  uint termination_index; 

  ArithmeticBrownianModel(float drift, float volatility, float spot_price,
                         float maturity, float step_size,
                         uint16_t number_of_steps,
                         std::vector<float> &wiener_process,
                         std::vector<float> &paths, std::vector<float> &stocks);

  ~ArithmeticBrownianModel();

  void print(void);
  void generate_path(void);
  void generate_stock_price(void);
  void write_csv(std::string filename, std::vector<data_column> dataset);
};


#pragma once 
class GeometricBrownianModel {
public: 
 // asset related
  float drift, volatility, spot_price;

  // time related.
  float maturity, step_size;
  uint16_t number_of_steps;
  std::vector<float> path_to_termination; 
  uint termination_index; 

  // indexed values.
  std::vector<float> wiener_process, paths, stocks;


  GeometricBrownianModel(float drift, float volatility, float spot_price,
                         float maturity, float step_size,
                         uint16_t number_of_steps,
                         std::vector<float> &wiener_process,
                         std::vector<float> &paths, std::vector<float> &stocks);

  ~GeometricBrownianModel();

  void print(void);
  void generate_path(void);
  void generate_stock_price(void);
  void write_csv(std::string filename, std::vector<data_column> dataset);
};