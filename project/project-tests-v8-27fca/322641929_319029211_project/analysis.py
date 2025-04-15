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

def sol_to_clusters(clustering_sol, data_points, num_clusters):
    """
    Convert the clustering solution to a list of clusters.
    Params:
      - clustering_sol: List of cluster assignments.
      - data_points: np array of the data points.
    Returns:
      - List of clusters.
    """
    #print(clustering_sol)
    clusters = []
    for i in range(num_clusters):
        cluster = [data_points[j] for j in range(len(data_points)) if clustering_sol[j] == i]
        clusters.append(cluster)
    return clusters

def calc_mean_dist(data_point, cluster):
    dist_list = []
    data_point_arr = np.array(data_point)
    for pnt in cluster:
        pnt_arr = np.array(pnt)
        if not np.array_equal(pnt_arr, data_point_arr):            
            dist = np.linalg.norm(data_point_arr - pnt_arr)
            dist_list.append(dist)
    dist_vec = np.array(dist_list)
    return np.mean(dist_vec) if dist_vec.size > 0 else 0

def calc_min_mean_dist(data_point, other_clusters):
    dist_list = []
    for cluster in other_clusters:
        if cluster != []:
            dist_list.append(calc_mean_dist(data_point, cluster))
    return min(dist_list)    
    

def calc_silhouette(data_point, cluster, other_clusters):
    a = calc_mean_dist(data_point, cluster)
    b = calc_min_mean_dist(data_point, other_clusters)
    return (b-a)/max(a,b)


def calc_score_symnmf(clusters):
    coeff_list = []
    for cluster in clusters:
        other_clusters = [clust for clust in clusters if clust != cluster]
        for pnt in cluster:                      
            coeff_list.append(calc_silhouette(pnt, cluster, other_clusters))
    coeff_list = np.array(coeff_list)
    return np.mean(coeff_list)

def calc_score_kmneans(input_file_path, k):
    pass



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
        delta_mu = euclid_dist(cent1, cent2)
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
    print("clustering_sol:", clustering_sol)
    nmf_score = silhouette_score(data_points, clustering_sol)
    kmeans_score = silhouette_score(data_points, dots_to_cents_map)
    print("nmf: ", nmf_score)
    print("kmeans: ", kmeans_score)


def main(args):
    k, file_name = args[1:]
    k = int(k)
    try:
        X = pd.read_csv(file_name, header=None)
    except IOError:
        print("An Error Has Occurred")
        print("Error reading file in python")
        return
    compare(X.values.tolist(), k)

main(sys.argv)