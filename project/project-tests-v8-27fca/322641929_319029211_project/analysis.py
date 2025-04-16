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
    Parameters:
      - H: Pandas DataFrame of the cluster assignments.
    Returns:
      - List of cluster assignments.
    """
    pd_H = pd.DataFrame(H)
    return pd_H.idxmax(axis=1)

def init_centroids(K, X):
    """
    Initialize the first K centroids from the dataset.
    Parameters:
      - K: Number of clusters.
      - X: Dataset as a list or array of data points.
    Returns:
      - List of the first K data points as initial centroids.
    """
    return [X[i] for i in range(K)]

def assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map):    
    """
    Assign a data point to the closest centroid based on Euclidean distance.
    Parameters:
      - vec_xi: The data point to assign.
      - i: Index of the data point.
      - centroids: List of current centroid vectors.
      - cent_to_dots_map: Dictionary mapping centroid indices to data points.
      - dot_to_cent_map: List mapping data point indices to centroid assignments.
    Returns:
      - None (updates cent_to_dots_map and dot_to_cent_map in-place).
    """
    distances = [np.linalg.norm(np.array(vec_xi) - np.array(c)) for c in centroids]
    closest_centroid = int(np.argmin(distances))
    cent_to_dots_map[closest_centroid].append(vec_xi)
    dot_to_cent_map[i] = closest_centroid

def update_centroids(centroids, cent_to_dots_map):
    """
    Update centroids by computing the mean of assigned data points.
    Parameters:
      - centroids: List of current centroid vectors (to be updated).
      - cent_to_dots_map: Dictionary mapping centroid indices to data points.
    Returns:
      - None (updates centroids in-place).
    """
    for i, dots in cent_to_dots_map.items():
        if dots:
            centroids[i] = np.mean(dots, axis=0)

def check_convergence(centroids, prev, eps):
    """
    Check if all centroids have moved less than epsilon.
    Parameters:
      - centroids: List of updated centroid vectors.
      - prev: List of centroid vectors from the previous iteration.
      - eps: Threshold for convergence.
    Returns:
      - True if converged, False otherwise.
    """
    return all(np.linalg.norm(np.array(c1) - np.array(c2)) < eps for c1, c2 in zip(centroids, prev))


def run_kmeans(K, datapoints, max_iter):
    """
    Run the K-means clustering algorithm.
    
    Parameters:
      - K: Number of clusters.
      - datapoints: List or array of data vectors.
      - max_iter: Maximum number of iterations.
    
    Returns:
      - Final centroids as a list of lists.
      - Cluster assignment list for each datapoint.
    """
    eps = 0.0001
    centroids = init_centroids(K, datapoints)
    cent_to_dots_map = {cent: [] for cent in range(K)}
    dot_to_cent_map = [0] * len(datapoints)
    for _ in range(max_iter):
        prev = [cent.copy() for cent in centroids]
        # Clear cents to dots map (clear assignments to clusters)
        cent_to_dots_map = {cent: [] for cent in cent_to_dots_map}
        for i, vec_xi in enumerate(datapoints):
            assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map)
        update_centroids(centroids, cent_to_dots_map)
        if check_convergence(centroids, prev, eps):
            break
    centroids_list = [arr.tolist() if isinstance(arr, np.ndarray) else arr for arr in centroids]
    return centroids_list, dot_to_cent_map

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
    """
    Perform Symmetric Matrix Factorization (SymNMF) on the input data.
    Parameters:
      - k: Number of clusters.
      - X: 2D list or numpy array representing the input similarity matrix.
    Returns:
      - H_next: The final matrix H after running SymNMF.
    """
    W = snmf.norm(X, 0)
    n = len(X)
    H = init_H(W, n, k)
    H_next = snmf.symnmf(H.values.tolist(), W, k, 0)
    return H_next

def compare(data_points, k):
    """
    Compare clustering performance between K-Means and SymNMF using the Silhouette Score.
    Run both symnmf and kmeans with the given input points.
    Parameters:
      - data_points: List or numpy array of data points.
      - k: Number of clusters.
    Returns:
      - None (prints silhouette scores to stdout).
    """
    centroids, dots_to_cents_map = run_kmeans(k, data_points, max_iter=300)
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