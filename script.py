from flexauto import AthenaAutocallable, ABM, GBM, AthenaResult
import torch
import torch.nn as nn 
from torch.nn import Module
from torch.distributions.log_normal import LogNormal
import numpy as np
import os
import matplotlib.pyplot as plt
import statistics
import math
from tqdm import tqdm
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

def run():
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
        "inception_spot":50.0,
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
        prices.append(gbm.getStockPath()[-1])    
        values.append(result.getPrice())
        results.append(result)
        path_to_terms.append(gbm.getTermPath())

    return prices


x = torch.tensor(run())
theta = torch.tensor([0.05, 0.07])
def obj_function(theta: torch.tensor):
    mu = theta[0]
    sig = theta[1]
    t = torch.tensor(5.0)
    S0 = torch.tensor(50) 
    loc = torch.log(S0) + (mu  - 0.5 * torch.pow(sig, 2)) * t
    scale = sig * torch.pow(t,0.5)

    m = LogNormal(loc, scale)
    return -m.log_prob(x).sum()
thetas_in_mu = []
thetas_in_mu_example = torch.linspace(0.04,0.06,50)

scores = []

for mu_ in thetas_in_mu_example:
    thetas_in_mu.append(torch.tensor([mu_,theta[1]]))
    scores.append(obj_function(theta=thetas_in_mu[-1]))

plt.plot(thetas_in_mu_example.flatten(), scores)
plt.savefig('mle.png')
theta = torch.tensor([0.05, 0.07])

log_like = obj_function(theta)
mu = torch.tensor(0.01, requires_grad =True)
sig = torch.tensor(0.02, requires_grad = True)


lr = 1e-2
optimizer = torch.optim.Adam([mu, sig], lr=lr)
max_iterations = 10000
loss = torch.zeros(max_iterations)
tq_obj = tqdm(range(max_iterations), desc=" Model Training")

# Gradient Iterations
for it in tq_obj:
    optimizer.zero_grad()
    cost = obj_function([mu, sig])
    cost.backward()
    optimizer.step()
    loss[it] = cost
    tqdm.write(f"Loss {loss[it]}  λ: {lr} mu: {mu}, sig: {sig}")  

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

