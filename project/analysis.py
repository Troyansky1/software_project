import math
import sys
import numpy as np
import pandas as pd
import symnmf as snmf

def derive_clustering_sol(H):
    """
    Derive the clustering solution from H.
    Params:
      - H: Pandas DataFrame of the cluster assignments.
    Returns:
      - List of cluster assignments.
    """
    return H.idxmax(axis=1)

def sol_to_clusters(clustering_sol, data_points):
    """
    Convert the clustering solution to a list of clusters.
    Params:
      - clustering_sol: List of cluster assignments.
      - data_points: Pandas DataFrame of the data points.
    Returns:
      - List of clusters.
    """
    #print(data_points)
    #print(clustering_sol)
    clusters = []
    for i in range(len(clustering_sol)):
        cluster_num = clustering_sol[i]
        cluster = [data_points[j] for j in range(len(data_points)) if clustering_sol[j] == cluster_num]
        clusters.append(cluster)
    return clusters

def calc_mean_dist(data_point, cluster):
    dist_list = []
    for pnt in cluster:
        dist = np.linalg.norm(data_point - pnt)
        if dist != 0:
            dist_list.append(dist)
    dist_vec = np.array(dist_list)
    return np.mean(dist_vec)

def calc_min_mean_dist(data_point, other_clusters):
    dist_list = []
    for cluster in other_clusters:
        dist_list.append(calc_mean_dist(data_point, cluster))
    return min(dist_list)    
    

def calc_silhouette(data_point, cluster, other_clusters):
    a = calc_mean_dist(data_point, cluster)
    b = calc_min_mean_dist(data_point, other_clusters)
    return (b-a)/max(a,b)

"""
def calc_score_symnmf(data_points, clustering_sol, clusters):
    coeff_list = []
    for idx, pnt in enumerate(data_points):
        cluster_num = clustering_sol[idx]
        cluster = [data_points[i] for i in range(len(data_points)) if clustering_sol[i] == cluster_num]
        other_clusters = [data_points[i] for i in range(len(data_points)) if i != idx] 
        coeff_list.append(calc_silhouette(pnt, cluster, other_clusters))
    coeff_list = np.array(coeff_list)
    return np.mean(coeff_list)
"""

def calc_score_symnmf(clusters):
    coeff_list = []
    for cluster in clusters:
        for pnt in cluster:
            other_clusters = [clust for clust in clusters if clust != cluster]
            coeff_list.append(calc_silhouette(pnt, cluster, other_clusters))
    coeff_list = np.array(coeff_list)
    return np.mean(coeff_list)

def calc_score_kmneans(input_file_path, k):
    pass


def compare(data_points, k):
    centroids = run_kmeans(k, data_points.values.tolist(), iter=300)
    H = run_symnmf(k, data_points.values.tolist())
    H = pd.DataFrame(H)
    clustering_sol = derive_clustering_sol(H).tolist()
    print(clustering_sol)
    clusters = sol_to_clusters(clustering_sol, data_points)
    print(calc_score_symnmf(clusters))


def init_centroids(K, X):
    centroids = []
    datapoints = X
    for i in range(K):
        centroids.append(datapoints[i])   
    return centroids

def euclid_dist(vector1, vector2):
    point1 = np.array(vector1)
    point2 = np.array(vector2)
    return np.linalg.norm(point1 - point2)

def assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map):
    min_dist = float('inf')
    min_cent = 0
    for j, vec_cent in enumerate(centroids):
        dist = euclid_dist(vec_xi, vec_cent)
        if (dist <= min_dist):
            min_dist = dist
            min_cent = j
    cent_to_dots_map[min_cent].append(vec_xi)
    dot_to_cent_map[i] = centroids[min_cent]

def update_centroids(centroids, cent_to_dots_map):
    for i in range(len(centroids)):
        all_coords = np.array(cent_to_dots_map[i])
        centroids[i] = np.mean(all_coords, axis=0)

def clear(cent_to_dots_map):
    for key in cent_to_dots_map:
        cent_to_dots_map[key] = []

def convergence(centroids, prev, eps):
    for cent1, cent2 in zip(centroids, prev):
        delta_mu = euclid_dist(cent1, cent2)
        if (delta_mu >= eps):
            return False
    return True

def run_kmeans(K, datapoints, iter):
    eps = 0.0001
    centroids = init_centroids(K, datapoints)
    cent_to_dots_map = {}
    for i in range(len(centroids)):
        cent_to_dots_map[i] = []
    dot_to_cent_map = {}
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
    return centroids

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
    W = snmf.norm(X)
    n = len(X)
    H = init_H(W, n, k)
    H = snmf.symnmf(H.values.tolist(), W, k, 0)
    return H

def main(args):
    k, file_name = args[1:]
    k = int(k)
    try:
        X = pd.read_csv(file_name, header=None)
    except IOError:
        print("An Error Has Occurred")
        print("Error reading file in python")
        return
    compare(X, k)

main(sys.argv)