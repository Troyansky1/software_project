import math
import sys
import numpy as np
import pandas as pd
import symnmf as snmf

np.random.seed(1234)

def init_H(W, n, k):
    """
    Randomly initialize Pandas DataFrame H with values from the interval [0, 2 * sqrt(m/k)].
    Parameters:
      - W: Pandas DataFrame of the normalized similarity matrix (n * n).
      - n: Number of data points (rows).
      - k: Number of clusters (columns).
    Returns:
      - Pandas DataFrame H.
    """
    if isinstance(W, list):  # Convert list of lists to DataFrame
        W = pd.DataFrame(W)
        m = W.values.mean()
        H = pd.DataFrame(np.random.uniform(0, 2*math.sqrt(m/k), size=(n, k)))
        return H
    else:
        pass


def deploy(goal, X, k):
    """
    Run the goal received as argument, and print the result.
    Parameters:
      - goal: A string indicating the goal to execute. It can be "symnmf","sym", "ddg", or "norm".
      - X: The input data.
      - k: Number of clusters (columns).
    Returns:
        - None.
    """
    if (goal == "symnmf"):
        W = snmf.norm(X, 0)
        n = len(X)
        H = init_H(W, n, k)
        snmf.symnmf(H.values.tolist(), W, k, 1)
    elif (goal == "sym"):
        snmf.sym(X)
    elif (goal == "ddg"):
        snmf.ddg(X)
    elif (goal == "norm"):
        snmf.norm(X, 1)
        
    return

def validate_input(K, goal, filename):
    """
    Validates the user arguments.
    Parameters:
      - K: Number of clusters.
      - goal: A string indicating the goal to execute. It must be one of these: "symnmf","sym", "ddg", or "norm".
      - filename: The input filename.      
    Returns:
      - A boolean, True if the arguments are valid, else False.
    """
    try:
        f = open(filename, "r")
        num_points = sum(1 for _ in f)  
        if (goal not in ["sym", "norm", "ddg", "symnmf"]):
            return False 
        if (not K.isdigit()): 
            return False 
        if (goal == "symnmf" and int(K) <= 1 or int(K) >= num_points):            
            return False                   
        f.close()
        return True
    except IOError: 
        return False


def main(args):
    """
    Validates the arguments and runs the goal with the given parameters.
    prints an error message if there is an error.
    Parameters:
      - arg[1] K: Number of clusters.
      - arg[2] goal: A string indicating the goal to execute. Can be "symnmf","sym", "ddg", or "norm".
      - arg[3] filename: The input filename.      
    Returns:
        - None.   
    """
    if (len(args) == 4):
        k, goal, file_name = args[1:]
        if not validate_input(k, goal, file_name):
            print("An Error Has Occurred")
            return
        k = int(k)
        try:
            X = pd.read_csv(file_name, header=None)
            if X.empty:
                print("An Error Has Occurred")
                return
            if not np.issubdtype(X.values.dtype, np.floating):
                print("An Error Has Occurred")
                return
        except IOError:
            print("An Error Has Occurred")
            return
        deploy(goal, X.values.tolist(), k)
    else:
        print("An Error Has Occurred")
        return

main(sys.argv)