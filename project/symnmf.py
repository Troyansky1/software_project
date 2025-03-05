import math
import sys
import numpy as np
import pandas as pd
import symnmf as snmf

np.random.seed(1234)

def init_H(W, k, n):
    """
    Randomly initialize H with values from the interval [0, 2 ∗ sqrt(m/k)].
    Params:
      - W: Pandas DataFrame of the normalized similarity matrix (n * n).
      - k: Number of clusters (columns).
      - n: Number of data points (rows).
    Returns:
      - Pandas DataFrame H.
    """
    m = W.values.mean()
    H = pd.DataFrame(np.random.uniform(0, 2*math.sqrt(m/k), size=(n, k)))
    return H


def deploy(goal, X, k):
    if (goal == "symnmf"):
        W = snmf.norm(X)
        H = init_H(W, k)
        snmf.symnmf(H, W)
    elif (goal == "sym"):
        snmf.sym(X)
    elif (goal == "ddg"):
          snmf.ddg(X)
    elif (goal == "norm"):
        snmf.norm(X)
    return

def main(args):
    k, goal, file_name = args[1:]
    k = int(k)
    try:
        X = pd.read_csv(file_name, header=None)
    except IOError:
        print("An Error Has Occurred")
        return
    print(X)
    deploy(goal, X, k)

main(sys.argv)