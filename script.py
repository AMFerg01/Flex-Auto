from flex import pyAthenaAutocallable, pyGBM, AthenaResult

import numpy as np
import os
import matplotlib.pyplot as plt

def clean_dir(name):
    if not os.path.exists(name):
        os.mkdir(name)
    else:
        os.system(f"rm {name}/*")

@staticmethod
def plot_athena_result(result: AthenaResult) -> None:
    """-- plots the result of an athena autocallable"""
    price = result.getPrice()
    title_str = (
        f"Athena Price {price:.2f} | Terminated: {result.getTermStatus()}"
    )
    ac_barrier = result.getAutocallBarrier()
    exit_barrier = result.getExitBarrier()
    kill_barrier = result.getKillBarrier()

    stock_path = result.getUnderlyingPath()
    time = np.linspace(0.0, result.getGbmMaturity(), result.getNumSteps() + 1)[
        : result.getTermIndex()
    ]
    if time.shape[0] != stock_path[0]:
        stock_path = stock_path[0 : time.shape[0]]

    # plot underlying path
    plt.plot(time, stock_path)
    plt.title(title_str)

    # plot observation dates.
    for obs_date in result.getObsDates():
        plt.axvline(obs_date, linestyle="--")

    # plot maturity date.
    plt.axvline(result.getMaturity())

    plt.axhline(result.getAutocallBarrier() * result.getInceptionSpot(), color="black")
    plt.axhline(result.getKillBarrier() * result.getInceptionSpot(), color="red")
    plt.axhline(result.getExitBarrier() * result.getInceptionSpot(), color="green")

    plt.xlabel("tte")
    plt.ylabel("spot")
    plt.savefig('plot.png')

if __name__ == "__main__":
    print("Running script")

    riskfree_rate = 0.05

    experiment_configuration = {
        "drift": 0.05,
        "volatility": 0.2,
        "spot_price": 1.0,
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
        result = athena.price_path(gbm)
        prices.append(result.getPrice())
        # print(result.getPrice())
        results.append(result)

    plt.figure(figsize=(10,6))

    plt.hist(prices)
    plt.savefig("athena_prices.png")
    plt.clf()

    print("Plotting all")

    for result in results:
        plot_athena_result(result)

    plt.savefig(f"athena_examples.png", dpi=100)
    plt.close()
    plt.clf()

    print("Plotting Maturity")
    athena_maturity_examples = [res for res in results if res.getTermStatus() == 'maturity']

    clean_dir('maturity')

    for i, result_example in enumerate(athena_maturity_examples):
        plot_athena_result(result_example)
        plt.savefig(f'maturity/athena_path_maturity_{i}_{result_example.getTermStatus()}.png', dpi=100)
        plt.close()
        plt.clf()

    athena_kill_barriers = [res for res in results if 'kill' in res.getTermStatus()]

    print("Plotting kill")

    clean_dir('kill')
    for i, result_example in enumerate(athena_kill_barriers):
        plot_athena_result(result_example)
        plt.savefig(f'kill/athena_path_kill_{i}_{result_example.getTermStatus()}.png', dpi=100)
        plt.close()
        plt.clf()

    print("Plotting AC")
    athena_ac_barriers = [res for res in results if 'ac-barrier' in res.getTermStatus()]

    clean_dir('ac')
    for i, result_example in enumerate(athena_ac_barriers):
        plot_athena_result(result_example)
        plt.savefig(f'ac/athena_path_ac_{i}_{result_example.getTermStatus()}.png', dpi=100)
        plt.close()
        plt.clf()

    athena_exit_barriers = [res for res in results if 'exit' in res.getTermStatus()]

    print("Plotting exits")
    clean_dir('exit')
    for i, result_example in enumerate(athena_exit_barriers):
        plot_athena_result(result_example)
        plt.savefig(f'exit/athena_path_exit_{i}_{result_example.getTermStatus()}.png', dpi=100)
        plt.close()
        plt.clf()
