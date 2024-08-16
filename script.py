from flexauto import AthenaAutocallable, ABM, GBM, AthenaResult
import torch
import torch.nn as nn 
from torch.nn import Module
import numpy as np
import os
import matplotlib.pyplot as plt
import statistics
import math
import scipy
from scipy.stats import lognorm
def clean_dir(name):
    if not os.path.exists(name):
        os.mkdir(name)
    else:
        os.system(f"rm -rf {name}")

# def LE(params, T, St, S0):
#     mu = params[0]
#     sigma = params[1]
#     n = len(S0)
#     l = np.log(np.array(St)/np.array(S0))
#     value = - (n * T * sigma ** 2)/8 + (n * mu * T)/2 - (1/(2 * T * sigma ** 2)) * sum(l**2) + (mu/(sigma ** 2)) * sum(l) - (n * T * mu ** 2)/(2 * sigma ** 2) - (n * math.log(sigma)) 
#     return value

# def find_params(params, T, St, S0):
#     gd = torch.optim.Adam([params], lr=1e-2, maximize=True)
#     hist = []
    
    # for i in range(100000):
    #     gd.zero_grad()
    #     obj = LE(params, T, St, S0) ### Maybe should be -LE?
    #     obj.backward()
    #     print("epoch: " + str(i) + ", LogL: " + str(obj.item()) + ", mu: " + str(params[0].item()) + ", sigma: " + str(params[1].item()))
    #     hist.append(obj.item())
    #     gd.step()
        


def plot_athena_result(result: AthenaResult, color, term_path) -> None:
    """-- plots the result of an athena autocallable"""
    price = result.getPrice()
    title_str = (
        f"Athena Price {price:.2f} | Terminated: {result.getTerminationStatus()}"
    )
    stock_path = result.getUnderlyingPath()


    time = np.linspace(0.0, (len(term_path)/1000) * result.getMaturity(), len(term_path))
    if time.shape[0] != stock_path[0]:
        stock_path = stock_path[0 : time.shape[0]]

    # plot underlying path
    plt.plot(time, term_path, color=color)
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
    # plt.savefig('plot.png')

if __name__ == "__main__":
    print("Running script")


    riskfree_rate = 0.05

    experiment_configuration = {
        "drift": 0.05,
        "volatility": 0.07,
        "spot_price": 50.0,
        "maturity": 5.0,  # years
        "step_size": 0.001,
        "number_of_steps": 1000
    }

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
        "inception_spot":501.0,
    }

    athena = AthenaAutocallable(*athena_configuration.values())


    prices = []
    statuses = []
    results = []
    path_to_terms = [] 
    values = []

    for i in range(100000):
        gbm = GBM(*experiment_configuration.values())
        gbm.generate_stock_price()
        result = athena.price_gbm(gbm)
        if(gbm.getTermIndex() > 900):
            prices.append(gbm.getTermPath()[-1])    
        values.append(result.getPrice())
        results.append(result)
        path_to_terms.append(gbm.getTermPath())

    mu = experiment_configuration["drift"]
    sigma = experiment_configuration["volatility"]


    plt.hist(prices, bins=np.linspace(40, 100, 200), density=True)
    plt.xscale("log")
    a, b, c = lognorm.fit(prices)
    real_pdf = scipy.stats.lognorm.pdf(np.linspace(40, 100, 200), a, b, c)
    print(a, mu - (sigma ** 2)/2)
    # exp_pdf = scipy.stats.lognorm.pdf(np.linspace(30, 100, 100), )
    data = np.sort(prices)
    plt.plot(np.linspace(40, 100, 200), real_pdf, 'r')
    # plt.plot(np.linspace(30, 100, 100), exp_pdf, 'g')
    plt.savefig("dist.png")
    plt.clf()
    # plt.figure(figsize=(16,12))

    # for result, term_path in zip(results, path_to_terms): 
    #     status = result.getTerminationStatus()
    #     if status == "AC + COUPON": 
    #         plot_athena_result(result, color='black', term_path=term_path)
    #     if status == 'KILL':
    #         plot_athena_result(result, color = 'red', term_path=term_path)
    #     if status == 'EXIT + COUPON':
    #         plot_athena_result(result, color = 'green', term_path=term_path)
    #     if status == 'MATURITY':
    #         plot_athena_result(result, color = 'blue', term_path=term_path)
    #     del status 
    # plt.title("Autocallable Example Paths")
    # plt.legend(handles=[
    # plt.Line2D([0], [0], color='black', lw=2, label='AUTOCALLED + COUPON'),
    # plt.Line2D([0], [0], color='red', lw=2, label='KILL'),
    # plt.Line2D([0], [0], color='green', lw=2, label='EXIT + COUPON'),
    # plt.Line2D([0], [0], color='blue', lw=2, label='MATURITY'),
    # plt.Line2D([0], [0], color='#1f77b4', lw=2, linestyle='--', label='OBSERVATION DATES')
    # ])    
    # plt.savefig('example.png')
    # plt.clf()
    # num_paths = []
    # std_devs = []
    # means = []
    # for i in [100,1000,10000]:
    #     a1 = np.mean(prices[0:i])
    #     a1_std = np.std(prices[0:i]) / i
    #     std_devs.append(a1_std)
    #     num_paths.append(i)
    #     means.append(a1)

    # plt.loglog(num_paths, std_devs)
    # plt.title("Standard Deviation vs Number of Paths")
    # plt.xscale("log")
    # plt.yscale("log")
    # plt.xlabel("Number of Paths")
    # plt.ylabel("Standard Deviation")
    # plt.savefig("stdev.png")
    # plt.clf() 

    # plt.plot(num_paths, means)
    # plt.title("Mean of Prices vs Number of Paths")
    # plt.xscale("log")
    # plt.xlabel("Number of Paths")
    # plt.ylabel("Average Price")
    # plt.savefig("mean.png")
    # plt.clf()

