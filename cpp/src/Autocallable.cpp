#include "Autocallable.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

#define quote "\""
#define write_json_line(key, value) quote << key << quote << ": " << value << ", \n"
#define write_json_line_last(key, value) quote << key << quote << ": " << value << "\n"
#define write_json_line_last_wquote(key, value) quote << key << quote << ": " << quote << value << quote << "\n"


#define log_vec(vec) \
    { \
        std::cout << #vec << ": ["; \
        for (size_t i = 0; i < vec.size(); ++i) { \
            std::cout << vec[i]; \
            if (i != vec.size() - 1) std::cout << ", "; \
        } \
        std::cout << "]" << std::endl; \
    }/* checks if a file exists with the specified name.


otherwise return false.
*/
bool file_exists(const std::string &name)
{
  // try to read, if you can then just close it and return boolean.
  std::FILE *file = std::fopen(name.c_str(), "r");
  if (file)
  {
    fclose(file);
    return true;
  }
  else
  {
    return false;
  }
}

AthenaResult::AthenaResult(float coupon_barrier, float autocall_barrier, float exit_barrier, float kill_barrier, float maturity,
                           const std::vector<float> &observation_dates,
                           float autocall_value, float coupon_value, float kill_value,
                           float inception_spot, const std::vector<float> &underlying_path)
    : coupon_barrier(coupon_barrier),
      autocall_barrier(autocall_barrier),
      exit_barrier(exit_barrier),
      kill_barrier(kill_barrier),
      maturity(maturity),
      observation_dates(observation_dates),
      autocall_value(autocall_value),
      coupon_value(coupon_value),
      kill_value(kill_value),
      inception_spot(inception_spot),
      underlying_path(underlying_path),
      price(NAN) {
      this->termination_status = std::string("not available");
      };

AthenaResult::~AthenaResult() {};

/* generates a json dump of the result class with a file called
result.json
*/
void AthenaResult::generate_json_dump(void)
{

  std::string result_name("result.json");

  if (file_exists(result_name))
  {
    throw std::runtime_error("result.json already exists, consider deleting it");
  }

  std::ofstream file;
  file.open(result_name);

  // write open bracket.
  file << "{\n";

  file << write_json_line("coupon_barrier", coupon_barrier);
  file << write_json_line("autocall_barrier", autocall_barrier);
  file << write_json_line("exit_barrier", exit_barrier);
  file << write_json_line("kill_barrier", kill_barrier);
  file << write_json_line("maturity", maturity);
  file << write_json_line("autocall_value", autocall_value);
  file << write_json_line("coupon_value", coupon_value);
  file << write_json_line("kill_value", kill_value);
  file << write_json_line("inception_spot", inception_spot);
  file << write_json_line_last_wquote("price", std::to_string(price));

  // write close bracket.
  file << "}\n";

  file.close();
}

AthenaAutocallable::AthenaAutocallable(float coupon_barrier, float autocall_barrier,
                                       float autocall_value, float exit_barrier,
                                       float kill_barrier, float maturity,
                                       std::vector<float> &observation_dates,
                                       float coupon_value, float kill_value,
                                       float inception_spot)
    : coupon_barrier(coupon_barrier),
      autocall_barrier(autocall_barrier),
      autocall_value(autocall_value),
      kill_barrier(kill_barrier),
      exit_barrier(exit_barrier),
      maturity(maturity),
      observation_dates(observation_dates),
      coupon_value(coupon_value),
      kill_value(kill_value),
      inception_spot(inception_spot)
{

  // perform checks.
  this->termination_status = std::string("not run"); 
  this->preliminary_checks();
  this->check_ordering_of_barriers();
}

void AthenaAutocallable::preliminary_checks(void)
{
  if (autocall_value <= 0.0)
  {
    throw BadAutocallParameterException("Autocall value is non-positive");
  }

  if (exit_barrier <= 0.0)
  {
    throw BadAutocallParameterException("Exit barrier is non-positive value");
  }

  if (kill_barrier <= 0.0)
  {
    throw BadAutocallParameterException("Kill value is non-positive");
  }
  if (maturity <= 0.0)
  {
    throw BadAutocallParameterException("Maturity cannot be negative");
  }

  // check if monotonic increasing.
  float observation_date_difference;

  // loop through dates and calculate difference of lag 1,
  // check if the difference is monotonically increasing.
  for (auto p = observation_dates.begin(); p != observation_dates.end() - 1;
       p++)
  {
    observation_date_difference = *(p + 1) - *p;
    if (observation_date_difference < 0.0)
    {
      throw BadAutocallParameterException(
          "Observation dates are not monotonically increasing.");
    }
  } // end of observation date check

  // check if maturity is less than observation date.
  if (*(observation_dates.end()) >= maturity)
  {
    throw BadAutocallParameterException(
        "Last observation date is geq than maturity. ");
  } // end of maturity check.
} // end of preliminary checks.

void AthenaAutocallable::check_ordering_of_barriers()
{

  if (!(this->kill_barrier < this->coupon_barrier))
  {
    throw BadAutocallParameterException("kill barrier cannot be less than coupon barrier");
  }

  if (!(fabs(this->coupon_barrier - this->autocall_barrier) < __FLT_EPSILON__))
  {
    throw BadAutocallParameterException("coupon barrier should be at autocall barrier otherwise this is not an Athena autocallable");
  }

  if (this->autocall_barrier >= this->exit_barrier)
  {

    throw BadAutocallParameterException("autocall barrier should be below the exit barrier.");
  }
}

std::optional<AthenaResult> AthenaAutocallable::check_terminations(int i,
                                                                   int index,
                                                                   std::vector<float> stock_normalized,
                                                                   ArithmeticBrownianModel abm,
                                                                   bool maturity)
{

  AthenaResult result = AthenaResult(
      this->coupon_barrier,
      this->autocall_barrier,
      this->exit_barrier,
      this->kill_barrier,
      this->maturity,
      this->observation_dates,
      this->autocall_value,
      this->coupon_value,
      this->kill_value,
      abm.stocks[0], // inception_spot
      abm.stocks     // underlying_path
  );

  // Note: If you need to set the 'price' to infinity, you should do it separately as it isn't passed in the constructor.
  float price;
  result.price = std::numeric_limits<float>::infinity();

  // AC < EXIT CHECK
  if (
      (this->autocall_barrier <= stock_normalized[index]) &&
      (stock_normalized[index] < this->exit_barrier))
  {

    // price is calculated as stock initial * (AC_VALUE + number of coupons added)
    float coupon_multiplier = this->coupon_value * (i + 1);
    price = abm.stocks[0] * (this->autocall_value + coupon_multiplier);
    result.price = price;
    this->termination_status = std::string("AC + COUPON");
    result.termination_status = this->termination_status;

  }

  // EXIT CHECK
  if (stock_normalized[index] >= this->exit_barrier)
  {
    price = abm.stocks[index] * (1.0 + this->coupon_value * (i + 1));
    result.price = price;
    this->termination_status = std::string("EXIT + COUPON");
    result.termination_status = this->termination_status;
  }

  // KILL CHECK
  if (stock_normalized[index] <= this->kill_barrier)
  {
    price = abm.stocks[0] * this->kill_value;
    result.price = price;
    this->termination_status = std::string("KILL");
    result.termination_status = this->termination_status;

  }

  if (
      (this->kill_barrier < stock_normalized[index]) &&
      (stock_normalized[index] < this->autocall_barrier))
  {
    // check maturity condition.
    if (maturity)
    {
      price = abm.stocks[0];
      result.price = price;
      this->termination_status = std::string("MATURITY");
      result.termination_status = this->termination_status;

    }
    else
    {
      return std::nullopt;
    }
  }

  if (result.price == INFINITY)
  {

    // comment this out later for debugging
    // std::cout << "price: " << price << std::endl; 
    // std::cout << "price: " << result.price << std::endl; 
    // std::cout << "stock_normalized: " << stock_normalized[index] << "," << index << std::endl; 
    // std::cout << "term: " << result.termination_status << std::endl; 

    throw BadAutocallError("user should not be here, price is infinity");
  }
  else
  {
    return result;
  }
}

std::optional<AthenaResult> AthenaAutocallable::check_terminations(int i,
                                                                   int index,
                                                                   std::vector<float> stock_normalized,
                                                                   GeometricBrownianModel gbm,
                                                                   bool maturity)
{

  AthenaResult result = AthenaResult(
      this->coupon_barrier,
      this->autocall_barrier,
      this->exit_barrier,
      this->kill_barrier,
      this->maturity,
      this->observation_dates,
      this->autocall_value,
      this->coupon_value,
      this->kill_value,
      gbm.stocks[0], // inception_spot
      gbm.stocks     // underlying_path
  );

  // Note: If you need to set the 'price' to infinity, you should do it separately as it isn't passed in the constructor.
  float price;
  result.price = std::numeric_limits<float>::infinity();

  // AC < EXIT CHECK
  if (
      (this->autocall_barrier <= stock_normalized[index]) &&
      (stock_normalized[index] < this->exit_barrier))
  {

    // price is calculated as stock initial * (AC_VALUE + number of coupons added)
    float coupon_multiplier = this->coupon_value * (i + 1);
    price = gbm.stocks[0] * (this->autocall_value + coupon_multiplier);
    result.price = price;
    this->termination_status = std::string("AC + COUPON");
    result.termination_status = this->termination_status;

  }

  // EXIT CHECK
  if (stock_normalized[index] >= this->exit_barrier)
  {
    price = gbm.stocks[index] * (1.0 + this->coupon_value * (i + 1));
    result.price = price;
    this->termination_status = std::string("EXIT + COUPON");
    result.termination_status = this->termination_status;
  }

  // KILL CHECK
  if (stock_normalized[index] <= this->kill_barrier)
  {
    price = gbm.stocks[0] * this->kill_value;
    result.price = price;
    this->termination_status = std::string("KILL");
    result.termination_status = this->termination_status;

  }

  if (
      (this->kill_barrier < stock_normalized[index]) &&
      (stock_normalized[index] < this->autocall_barrier))
  {
    // check maturity condition.
    if (maturity)
    {
      price = gbm.stocks[0];
      result.price = price;
      this->termination_status = std::string("MATURITY");
      result.termination_status = this->termination_status;

    }
    else
    {
      return std::nullopt;
    }
  }

  if (result.price == INFINITY)
  {

    // comment this out later for debugging
    // std::cout << "price: " << price << std::endl; 
    // std::cout << "price: " << result.price << std::endl; 
    // std::cout << "stock_normalized: " << stock_normalized[index] << "," << index << std::endl; 
    // std::cout << "term: " << result.termination_status << std::endl; 

    throw BadAutocallError("user should not be here, price is infinity");
  }
  else
  {
    return result;
  }
}





AthenaResult AthenaAutocallable::price_abm(ArithmeticBrownianModel &abm)
{

  float price = INFINITY;

  auto result = AthenaResult(this->coupon_barrier,
                             this->autocall_barrier,
                             this->exit_barrier,
                             this->kill_barrier,
                             abm.maturity,
                             this->observation_dates,
                             this->autocall_value,
                             this->autocall_value, //  coupon value is also autocall value
                             this->kill_value,
                             this->inception_spot,
                             abm.stocks);

  result.price = price;

  // Normalize stock prices

  std::vector<float> stock_normalized(abm.stocks.size());
  
  for (size_t i = 0; i < abm.stocks.size(); ++i)
  {
    stock_normalized[i] = abm.stocks[i] / abm.stocks[0];
  }

  // Calculate time steps
  std::vector<float> time(abm.number_of_steps + 1);
  float time_step = abm.maturity / abm.number_of_steps;
  for (size_t i = 0; i <= abm.number_of_steps; ++i)
  {
    time[i] = i * time_step;
  }

  // Time index
  std::vector<uint> time_index(abm.number_of_steps + 1);
  for (uint i = 0; i <= abm.number_of_steps; ++i)
  {
    time_index[i] = i;
  }

  // Observation dates
  std::vector<float> observation_dates(this->observation_dates);
  std::vector<size_t> observed_time_index;

  // Determine if the observation dates are close to time steps
  std::vector<bool> obs_is_close(observation_dates.size(), false);

  for (size_t i = 0; i < observation_dates.size(); ++i)
  {
    for (size_t j = 0; j < time.size(); ++j)
    {
      if (fabs(observation_dates[i] - time[j]) < abm.step_size / 2)
      {
        obs_is_close[i] = true;
        observed_time_index.push_back((size_t)time_index[j]);
      }
    }
  }

  // go through observations
  for (size_t i = 0; i < observed_time_index.size(); ++i)
  {
    int index = observed_time_index[i];

    // throw std::runtime_error(index);

    // Call check_terminations function
    std::optional<AthenaResult> termination_value = this->check_terminations(
        i,
        index,
        stock_normalized,
        abm,
        false // maturity
    );

    // Continue if no termination value is returned
    if (!termination_value.has_value())
    {
      continue;
    }


    // Check if the termination_value contains a valid AthenaResult
    if (termination_value)
    {
      abm.termination_index = index;  
      return termination_value.value();
    }

    // Raise error if reached this point
    throw std::runtime_error("User should not be here");
  }

  // Check at maturity
  int index = -1; // This indicates the end of the period, i.e., maturity

  // Call check_terminations for the maturity condition
  std::optional<AthenaResult> termination_at_maturity = this->check_terminations(
      this->observation_dates.size(), // i is the number of observation dates
      index,
      stock_normalized,
      abm,
      true // maturity
  );

  // Continue if no termination value is returned
  if (!termination_at_maturity.has_value())
  {
    throw BadAutocallError("User should not be here, everything must be terminated at maturity");
  }

  // Check if the termination_value contains a valid AthenaResult
  if (termination_at_maturity)
  {
    abm.termination_index = index;  
    return termination_at_maturity.value();
  }

  return result;
}



AthenaResult AthenaAutocallable::price_gbm(GeometricBrownianModel &gbm)
{

  float price = INFINITY;

  auto result = AthenaResult(this->coupon_barrier,
                             this->autocall_barrier,
                             this->exit_barrier,
                             this->kill_barrier,
                             gbm.maturity,
                             this->observation_dates,
                             this->autocall_value,
                             this->autocall_value, //  coupon value is also autocall value
                             this->kill_value,
                             this->inception_spot,
                             gbm.stocks);

  result.price = price;

  // Normalize stock prices

  std::vector<float> stock_normalized(gbm.stocks.size());
  
  for (size_t i = 0; i < gbm.stocks.size(); ++i)
  {
    stock_normalized[i] = gbm.stocks[i] / gbm.stocks[0];
  }

  // Calculate time steps
  std::vector<float> time(gbm.number_of_steps + 1);
  float time_step = gbm.maturity / gbm.number_of_steps;
  for (size_t i = 0; i <= gbm.number_of_steps; ++i)
  {
    time[i] = i * time_step;
  }

  // Time index
  std::vector<uint> time_index(gbm.number_of_steps + 1);
  for (uint i = 0; i <= gbm.number_of_steps; ++i)
  {
    time_index[i] = i;
  }

  // Observation dates
  std::vector<float> observation_dates(this->observation_dates);
  std::vector<size_t> observed_time_index;

  // Determine if the observation dates are close to time steps
  std::vector<bool> obs_is_close(observation_dates.size(), false);

  for (size_t i = 0; i < observation_dates.size(); ++i)
  {
    for (size_t j = 0; j < time.size(); ++j)
    {
      if (fabs(observation_dates[i] - time[j]) < gbm.step_size / 2)
      {
        obs_is_close[i] = true;
        observed_time_index.push_back((size_t)time_index[j]);
      }
    }
  }

  // go through observations
  for (size_t i = 0; i < observed_time_index.size(); ++i)
  {
    int index = observed_time_index[i];

    // throw std::runtime_error(index);

    // Call check_terminations function
    std::optional<AthenaResult> termination_value = this->check_terminations(
        i,
        index,
        stock_normalized,
        gbm,
        false // maturity
    );

    // Continue if no termination value is returned
    if (!termination_value.has_value())
    {
      continue;
    }


    // Check if the termination_value contains a valid AthenaResult
    if (termination_value)
    {
      gbm.termination_index = index;
      std::vector<float> termination_path(gbm.stocks.begin(), gbm.stocks.begin() + index);
      gbm.path_to_termination = termination_path;  
      return termination_value.value();
    }

    // Raise error if reached this point
    throw std::runtime_error("User should not be here");
  }

  // Check at maturity
  int index = -1; // This indicates the end of the period, i.e., maturity

  // Call check_terminations for the maturity condition
  std::optional<AthenaResult> termination_at_maturity = this->check_terminations(
      this->observation_dates.size(), // i is the number of observation dates
      index,
      stock_normalized,
      gbm,
      true // maturity
  );

  // Continue if no termination value is returned
  if (!termination_at_maturity.has_value())
  {
    throw BadAutocallError("User should not be here, everything must be terminated at maturity");
  }

  // Check if the termination_value contains a valid AthenaResult
  if (termination_at_maturity)
  {

    gbm.termination_index = index; 
    gbm.path_to_termination = gbm.stocks; 
    return termination_at_maturity.value();
  }

  return result;
}
