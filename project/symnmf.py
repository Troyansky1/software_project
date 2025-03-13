import math
import sys
import numpy as np
import pandas as pd
import symnmfmodule as snmf

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
    if isinstance(W, list):  # Convert list of lists to DataFrame
        W = pd.DataFrame(W)
        m = W.values.mean()
        print(W)
        H = pd.DataFrame(np.random.uniform(0, 2*math.sqrt(m/k), size=(n, k)))
        return H
    else:
        pass


def deploy(goal, X, k):
    if (goal == "symnmf"):
        W = snmf.norm(X)
        n = len(X)
        H = init_H(W, k, n)
        snmf.symnmf(H.values.tolist(), W, k, 1)
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
        print("Error reading file in python")
        return
    deploy(goal, X.values.tolist(), k)

main(sys.argv)