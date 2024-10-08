# Flex-Auto

An open-source implementation of the Autocallable product found in [Guillaume, Tristan. "Autocallable Structured Products." *The Journal of Derivatives*, vol. 22, no. 3, 2015, pp. 73-94.](https://www.pm-research.com/content/iijderiv/22/3/73).

Currently features an Athena Autocallable product in continous time with a standard geometric Brownian motion model.

## What are Autocallables?

Autocallables are complex structured notes which track the trajectories of one or several underlying securities. On specific time points termed "observation dates", the autocallable may payout based on different conditions specified in the inception of the contract. There are numerous flavours of Autocallables which are bought and sold across financial instiutions worldwide. Flex-Auto serves as an open source tool for industry and academia to properly understand and analyze autocallable products.

## Codebase 

Flex-Auto is maintained as a `pybind11` Python package, with the underlying source code written entirely in C++. The code follows idiomatic C++ practices, favoring the use of vectors and iterators, which provide both efficiency and safety, over more manual, C-like approaches to performance optimization. We chose to be more verbose for the Brownian Motion models to achieve clarity, over performance. 

## Installation

This project uses cmake to build and facilitate the Python package `flexauto`. To install the library, run the following commands

```bash

# create cmake files
cmake .

# install python package flexauto 
make install 

# check to see if properly installed. 
python3 -c 'import flexauto; print(flexauto)'
# <module 'flexauto' from '/home/app_user/app/flexauto.cpython-310-x86_64-linux-gnu.so'>

```

## Contributions

- pybind11 wrapper, C++ extensions to Python, debugging, tests. [Aidan Ferguson](https://www.linkedin.com/in/aidan-m-ferguson/)
- model conception and development, C++ core implementation. [Nik Pocuca](https://www.linkedin.com/in/pocucan/)

## Roadmap

- [ ] Discounting with Interest
- [x] Heston Model [PR-7](https://github.com/AMFerg01/Flex-Auto/pull/7)
- [ ] NVIDIA CUDA Kernel Extension
- [ ] AMD ROCm Kernel Extension
- [ ] Date schedules using QuantLib
- [ ] Best of basket
- [ ] Lookback effect

## Docker

It is highly recommended that you use `docker` and `docker compose` to manage and execute this library.

To make the shell use

```bash
docker compose build shell 
```

Once built, we can instantiate the shell using:

```bash
docker compose run shell 
```

and you will be greeted with:

```bash
(ubuntu-jammy) /home/app_user/app $: 
```

Once the shell has been instantiated, see the section on [Installation](#installation) to build and install the python package.

## Examples

There is currently only one example called `script.py` which generates a series of paths using a Geometric Brownian Motion model, and calculates the average price at time `maturity`.

The following example runs each path in serial and calculates the price. There are more efficient ways to call the underling C++ code, but this python script is relatively easy to follow:

```python

# set up experiment
experiment_configuration = {
    "drift": 0.05,
    "volatility": 0.06,
    "spot_price": 1.0,
    "maturity": 5.0,  # years
    "step_size": 0.01,
    "number_of_steps": 1000
}

# set up structured note
athena_configuration = {
    "coupon_barrier": 1.1,
    "autocall_barrier": 1.1,
    "autocall_value": 1.0,
    "exit_barrier": 1.2,
    "kill_barrier": 0.8,
    "maturity": experiment_configuration["maturity"],
    "observation_dates": [1.0, 2.0, 3.0],
    "coupon_value": 0.05,
    "kill_value": 0.8,
    "inception_spot": 1.0,
}

athena = AthenaAutocallable(*athena_configuration.values())


# run 100 paths
prices = []
statuses = []
results = []
path_to_terms = [] 

for i in range(100):
    gbm = GBM(*experiment_configuration.values())
    gbm.generate_stock_price()
    result = athena.price_gbm(gbm)
    prices.append(result.getPrice())
    results.append(result)
    path_to_terms.append(gbm.getTermPath())

price_at_maturity = np.mean(prices)
print(f"Price is {price_at_maturity}")
```

Here are some visuals of a few example paths generated: 


![image](https://github.com/user-attachments/assets/d369bc0a-c8d2-4c7c-9338-9abd756e04e9)

The blue paths are the asset trajectories that made it till maturity, the red lines are when the asset was observed below the kill barrier on a particular observation date. The green lines are when the underlying broke through the exit barrier, and finally the black lines are when the contract was autocalled. 
Depending on the path taken, each payoff is calculated with respect to the observed date. 
