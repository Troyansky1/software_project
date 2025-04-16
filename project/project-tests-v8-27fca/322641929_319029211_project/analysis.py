import math
import sys
import numpy as np
import pandas as pd
import symnmf as snmf
from sklearn.metrics import silhouette_score

np.random.seed(1234)

def derive_clustering_sol(H):
    """
    Derive the clustering solution from H.
    Params:
      - H: Pandas DataFrame of the cluster assignments.
    Returns:
      - List of cluster assignments.
    """
    pd_H = pd.DataFrame(H)
    return pd_H.idxmax(axis=1)

def init_centroids(K, X):
    centroids = []
    datapoints = X
    for i in range(K):
        centroids.append(datapoints[i])   
    return centroids

def assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map):
    min_dist = float('inf')
    min_cent = 0
    for j, vec_cent in enumerate(centroids):
        dist = np.linalg.norm(np.array(vec_xi) - np.array(vec_cent))
        if (dist <= min_dist):
            min_dist = dist
            min_cent = j
    cent_to_dots_map[min_cent].append(vec_xi)
    dot_to_cent_map[i] = min_cent

def update_centroids(centroids, cent_to_dots_map):
    for i in range(len(centroids)):
        all_coords = np.array(cent_to_dots_map[i])        
        if (len(all_coords) != 0):
            centroids[i] = np.mean(all_coords, axis=0)

def clear(cent_to_dots_map):
    for key in cent_to_dots_map:
        cent_to_dots_map[key] = []

def convergence(centroids, prev, eps):
    for cent1, cent2 in zip(centroids, prev):
        delta_mu = np.linalg.norm(np.array(cent1)- np.array(cent2))
        if (delta_mu >= eps):
            return False
    return True

def convert_to_list(arrays):
    return [arr.tolist() if isinstance(arr, np.ndarray) else arr for arr in arrays]

def run_kmeans(K, datapoints, iter):
    eps = 0.0001
    centroids = init_centroids(K, datapoints)
    cent_to_dots_map = {}
    for i in range(len(centroids)):
        cent_to_dots_map[i] = []
    dot_to_cent_map = [_ for _ in range(len(datapoints))]
    conv_flag = False
    j = 0
    while ((not conv_flag) and (j < iter)):
        prev = []
        for cent in centroids:
            prev.append(cent.copy())
        clear(cent_to_dots_map)
        for i, vec_xi in enumerate(datapoints):
            assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map)
        update_centroids(centroids, cent_to_dots_map)
        conv_flag = convergence(centroids, prev, eps)
        j = j + 1
    
    return convert_to_list(centroids), dot_to_cent_map

def init_H(W, n, k):
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
        H = pd.DataFrame(np.random.uniform(0, 2*math.sqrt(m/k), size=(n, k)))
        return H
    else:
        pass

def run_symnmf(k, X):
    W = snmf.norm(X, 0)
    n = len(X)
    H = init_H(W, n, k)
    H_next = snmf.symnmf(H.values.tolist(), W, k, 0)
    return H_next

def compare(data_points, k):
    centroids, dots_to_cents_map = run_kmeans(k, data_points, iter=300)
    centroids = np.array(centroids)
    final_H = run_symnmf(k, data_points)
    clustering_sol = derive_clustering_sol(final_H).tolist()
    nmf_score = silhouette_score(data_points, clustering_sol)
    kmeans_score = silhouette_score(data_points, dots_to_cents_map)
    print("nmf:", f"{nmf_score:.4f}" )
    print("kmeans:", f"{kmeans_score:.4f}")

def validate_input(K, filename):
    """
    Validate the user arguments.
    Params:
      - K: Number of clusters.
      - filename: The input filename.      
    Returns:
      - A bolean, True if the arguments are valid, else False.
    """
    try:
        f = open(filename, "r")
        line_count = sum(1 for _ in f)  
        if (not K.isdigit()): 
            return False 
        if (int(K) <= 1 or int(K) >= line_count):            
            return False                   
        f.close()
        return True
    except IOError: 
        return False
    
def main(args):
    """
    Validates the arguments and runs the goal with the given parameters.
    prints an error message if there is an error.
    Params:
      - arg[1] K: Number of clusters.
      - arg[2] filename: The input filename.         
    """
    if (len(args) == 3):
        k, file_name = args[1:]
        if not validate_input(k, file_name):
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
        compare(X.values.tolist(), k)
    else:
        print("An Error Has Occurred")
        return

main(sys.argv)