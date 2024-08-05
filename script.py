from flexauto import AthenaAutocallable, ABM, AthenaResult

import numpy as np
import os
import matplotlib.pyplot as plt


if __name__ == "__main__":
    print("Running script")

    riskfree_rate = 0.05

    experiment_configuration = {
        "drift": 0.05,
        "volatility": 0.06,
        "spot_price": 1.2,
        "maturity": 5.0,  # years
        "step_size": 0.01,
        "number_of_steps": 1000,
        "wiener_process": np.random.randn(1000),
        "paths": np.random.randn(1000),
        "stocks": np.ones(1000),        

    }

    athena_configuration = {
        "coupon_barrier": 1.0,
        "autocall_barrier": 1.0,
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

    prices = []
    statuses = []
    results = []
    for i in range(1000):
        abm = ABM(*experiment_configuration.values())
        abm.generate_stock_price()
        result = athena.price_abm(abm)
        prices.append(result.getPrice())
        # print(result.getPrice())
        results.append(result)

    print(prices)

   