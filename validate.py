import torch 
from tqdm import tqdm
from flexauto import AthenaAutocallable, GBM
from torch.distributions.log_normal import LogNormal

# set up experiment
riskfree_rate = 0.05

experiment_configuration = {
    "drift": 0.05,
    "volatility": 0.06,
    "spot_price": 1.0,
    "maturity": 5.0,  # years
    "step_size": 0.01,
    "number_of_steps": int(5.0 / 0.01)
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
    "inception_spot": 1.0,
}

if __name__ == '__main__':

    athena = AthenaAutocallable(*athena_configuration.values())

    prices = []

    for i in range(10000):
        gbm = GBM(*experiment_configuration.values())
        gbm.generate_stock_price()
        result = athena.price_gbm(gbm)
        prices.append(gbm.getStockPath()[-1])

    # validate 
    x = torch.tensor(prices)

    def obj_function(theta: torch.tensor):
        mu = theta[0]
        sig = theta[1]
        t = torch.tensor(experiment_configuration['maturity'])
        S0 = torch.tensor(experiment_configuration['spot_price']) 
        loc = torch.log(S0) + (mu  - 0.5 * torch.pow(sig, 2)) * t
        scale = sig * torch.pow(t,0.5)

        m = LogNormal(loc, scale)
        return -m.log_prob(x).sum()

    mu = torch.tensor(0.01, requires_grad =True)
    sig = torch.tensor(0.02, requires_grad = True)


    lr = 1e-3
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
        tqdm.write(f"Loss {loss[it]}  λ: {lr} mu: {mu}, sig: {sig} | true parameters: {experiment_configuration['drift']}, {experiment_configuration['volatility']}")