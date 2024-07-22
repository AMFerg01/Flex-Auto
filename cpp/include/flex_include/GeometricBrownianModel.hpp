#include <cstdint>
#include <vector>
#include <iostream>

#pragma once
class GeometricBrownianModel
{
public:
  // asset related
  float drift, volatility, spot_price;

  // time related.
  float maturity, step_size;
  uint16_t number_of_steps;

  // indexed values.
  std::vector<float> wiener_process, paths, stocks;

  GeometricBrownianModel(float drift, float volatility, float spot_price,
                         float maturity, float step_size,
                         uint16_t number_of_steps,
                         std::vector<float> &wiener_process,
                         std::vector<float> &paths, std::vector<float> &stocks);

  ~GeometricBrownianModel();

  // print utilities
  void print(void);
  void print_path(unsigned int num_steps);
  void print_stocks(unsigned int num_steps);

  // workload methods.
  void generate_path(void);
  void compute_spot_prices(void);
};
