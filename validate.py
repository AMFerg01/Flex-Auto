import torch 
from torch.distributions.log_normal import LogNormal
import matplotlib.pyplot as plt 
from tqdm import tqdm


# experiment setup
mu = torch.tensor(0.05)
sig = torch.tensor(0.07) 
t = torch.tensor(5.0)
S0 = torch.tensor(50) 
loc = torch.log(S0) + (mu  - 0.5 * torch.pow(sig, 2)) * t
scale = sig * torch.pow(t,0.5)
m = LogNormal(loc, scale)
x_shape = (5000,1)
x = m.sample(x_shape)


def obj_function(theta: torch.tensor):
    mu = theta[0]
    sig = theta[1]
    t = torch.tensor(5.0)
    S0 = torch.tensor(50) 
    loc = torch.log(S0) + (mu  - 0.5 * torch.pow(sig, 2)) * t
    scale = sig * torch.pow(t,0.5)

    m = LogNormal(loc, scale)
    return -m.log_prob(x).sum()


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




# thetas_in_mu = []
# thetas_in_mu_example = torch.linspace(0.04,0.06,50)

# scores = []

# for mu_ in thetas_in_mu_example:
#     thetas_in_mu.append(torch.tensor([mu_,theta[1]]))
#     scores.append(obj_function(theta=thetas_in_mu[-1]))

# plt.plot(thetas_in_mu_example.flatten(), scores)
# plt.savefig('mle.png')





# thetas_in_mu = []
# thetas_in_mu_example = torch.linspace(0.06,0.08,50)

# scores = []

# for mu_ in thetas_in_mu_example:
#     thetas_in_mu.append(torch.tensor([theta[0],mu_]))
#     scores.append(obj_function(theta=thetas_in_mu[-1]))

# plt.plot(thetas_in_mu_example.flatten(), scores)
# plt.savefig('mle.png')


breakpoint()

# plt.hist(x.flatten(), bins=50) 
# plt.savefig('example.png')