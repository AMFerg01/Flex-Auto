from flex import pyAthenaAutocallable, pyGBM

import numpy as np
import os
import matplotlib.pyplot as plt

def clean_dir(name):
    if not os.path.exists(name):
        os.mkdir(name)
    else:
        os.system(f"rm {name}/*")


if __name__ == "__main__":
    print("Running script")

    riskfree_rate = 0.05

    experiment_configuration = {
        "drift": 0.05,
        "volatility": 0.2,
        "spot_price": 1.0,
        "maturity": 5.0,  # years
        "step_size": 1,
        "number_of_steps": 1000,
        "wiener_process": np.random.randn(1000),
        "paths": np.zeros(1000),
        "stocks": np.zeros(1000),        

    }

    athena_configuration = {
        "coupon_barrier": 1.0,
        "autocall_barrier": 1.0,
        "exit_barrier": 1.2,
        "kill_barrier": 0.8,
        "autocall_value": 1.0,
        "coupon_value": 0.05,
        "kill_value": 0.8,
        "maturity": experiment_configuration["maturity"],
        "observation_dates": [1.0, 2.0, 3.0],
        
    }

    athena = pyAthenaAutocallable(*athena_configuration.values())

    prices = []
    results = []
    for i in range(20):
        gbm = pyGBM(*experiment_configuration.values())
        gbm.generate_path()
        gbm.compute_spot_prices()
        result = athena.price_path(gbm)
        prices.append(result.getPrice())
        print(result.getPrice())
        results.append(result)

    plt.figure(figsize=(10,6))

    plt.hist(prices, range=[0.95, 1.1 ])
    plt.savefig("athena_prices.png")
    plt.clf()

    # print("Plotting all")

    # for result in results:
    #     athena.plot_athena_result(result)

    # plt.savefig(f"athena_examples.png", dpi=100)
    # plt.close()
    # plt.clf()

    # print("Plotting Maturity")
    # athena_maturity_examples = [res for res in results if res.termination_status == 'maturity']

    # clean_dir('maturity')

    # for i, result_example in enumerate(athena_maturity_examples):
    #     athena.plot_athena_result(result_example)
    #     plt.savefig(f'maturity/athena_path_maturity_{i}_{result_example.termination_status}.png', dpi=100)
    #     plt.close()
    #     plt.clf()

    # athena_kill_barriers = [res for res in results if 'kill' in res.termination_status]

    # print("Plotting kill")

    # clean_dir('kill')
    # for i, result_example in enumerate(athena_kill_barriers):
    #     athena.plot_athena_result(result_example)
    #     plt.savefig(f'kill/athena_path_kill_{i}_{result_example.termination_status}.png', dpi=100)
    #     plt.close()
    #     plt.clf()

    # print("Plotting AC")
    # athena_ac_barriers = [res for res in results if 'ac-barrier' in res.termination_status]

    # clean_dir('ac')
    # for i, result_example in enumerate(athena_ac_barriers):
    #     athena.plot_athena_result(result_example)
    #     plt.savefig(f'ac/athena_path_ac_{i}_{result_example.termination_status}.png', dpi=100)
    #     plt.close()
    #     plt.clf()

    # athena_exit_barriers = [res for res in results if 'exit' in res.termination_status]

    # print("Plotting exits")
    # clean_dir('exit')
    # for i, result_example in enumerate(athena_exit_barriers):
    #     athena.plot_athena_result(result_example)
    #     plt.savefig(f'exit/athena_path_exit_{i}_{result_example.termination_status}.png', dpi=100)
    #     plt.close()
    #     plt.clf()
