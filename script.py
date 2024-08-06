from flexauto import AthenaAutocallable, ABM, GBM, AthenaResult

import numpy as np
import os
import matplotlib.pyplot as plt
import statistics
def clean_dir(name):
    if not os.path.exists(name):
        os.mkdir(name)
    else:
        os.system(f"rm -rf {name}")


def plot_athena_result(result: AthenaResult, color) -> None:
    """-- plots the result of an athena autocallable"""
    price = result.getPrice()
    title_str = (
        f"Athena Price {price:.2f} | Terminated: {result.getTerminationStatus()}"
    )
    stock_path = result.getUnderlyingPath()
    time = np.linspace(0.0, result.getMaturity(), 1000)
    if time.shape[0] != stock_path[0]:
        stock_path = stock_path[0 : time.shape[0]]

    # plot underlying path
    plt.plot(time, stock_path, color=color)
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
        "volatility": 0.06,
        "spot_price": 1.0,
        "maturity": 5.0,  # years
        "step_size": 0.01,
        "number_of_steps": 1000
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
    for i in range(10000):
        gbm = GBM(*experiment_configuration.values())
        gbm.generate_stock_price()
        result = athena.price_gbm(gbm)

        prices.append(result.getPrice())
        results.append(result)

    # plt.savefig("athena_prices.png")
    # plt.clf()
    # print("Plotting Maturity")
    # athena_maturity_examples = [res for res in results if res.getTerminationStatus() == 'maturity']
    # clean_dir('maturity')
    # for i, result_example in enumerate(athena_maturity_examples):
    #     plot_athena_result(result_example, "blue")
    #     plt.savefig(f'maturity/athena_path_maturity_{i}_{result_example.getTerminationStatus()}.png', dpi=100)
    #     plt.close()
    #     plt.clf()
    # athena_kill_barriers = [res for res in results if 'KILL' in res.getTerminationStatus()]
    # print("Plotting kill")
    # clean_dir('kill')
    # for i, result_example in enumerate(athena_kill_barriers):
    #     plot_athena_result(result_example, "red")
    #     plt.savefig(f'kill/athena_path_kill_{i}_{result_example.getTerminationStatus()}.png', dpi=100)
    #     plt.close()
    #     plt.clf()
    # print("Plotting AC")
    # athena_ac_barriers = [res for res in results if 'AC + COUPON' in res.getTerminationStatus()]
    # clean_dir('ac')
    # for i, result_example in enumerate(athena_ac_barriers):
    #     plot_athena_result(result_example, "black")
    #     plt.savefig(f'ac/athena_path_ac_{i}_{result_example.getTerminationStatus()}.png', dpi=100)
    #     plt.close()
    #     plt.clf()   
    # athena_exit_barriers = [res for res in results if 'EXIT + COUPON' in res.getTerminationStatus()]
    # print("Plotting exits")
    # clean_dir('exit')
    # for i, result_example in enumerate(athena_exit_barriers):
    #     plot_athena_result(result_example, "green")
    #     plt.savefig(f'exit/athena_path_exit_{i}_{result_example.getTerminationStatus()}.png', dpi=100)
    #     plt.close()
    #     plt.clf()

    a1 = sum([r.getPrice()/100 for r in results[:100]])
    a2 = sum([r.getPrice()/1000 for r in results[:1000]])
    a3 = sum([r.getPrice()/5000 for r in results[:5000]])
    a4 = sum([r.getPrice()/10000 for r in results[:10000]])
    avg_price = [a1, a2, a3, a4]
    
    s1 = statistics.stdev([r.getPrice() for r in results[:100]])
    s2 = statistics.stdev([r.getPrice() for r in results[:1000]])
    s3 = statistics.stdev([r.getPrice() for r in results[:5000]])
    s4 = statistics.stdev([r.getPrice() for r in results[:10000]])
    stds = [s1, s2, s3, s4]
    plt.plot([100, 1000, 5000, 10000], stds)
    plt.title("Standard Deviation vs Number of Paths")
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Number of Paths")
    plt.ylabel("Standard Deviation")
    plt.savefig("stdev.png")