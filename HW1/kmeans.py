#!/usr/bin/env python3

import numpy as np
import sys

# each centroid and point is represented by its index in list centroids and datapoints respectively
# every vector is a list of coordinates
# data structures used:
# cent_to_dots_map - dictionary where keys are indexes of centroids and values are lists of vectors assigned to that centroid
# dot_to_cent_map - dictionary where keys are indexes of points and values are the centroid vector that dot is assigned to

def print_centroids(centroids):
    for cent in centroids:
        exp = ""
        for c in cent:
            exp += '{:.4f}'.format(c) + ","
        print(exp[:-1])
        

def validate_input(K, iter, filename):
    try:
        f = open(filename, "r")
        line_count = sum(1 for _ in f)
        if (K < 1 or K > line_count or np.floor(K) != K):
            print("Invalid number of clusters!")
            return False
        if (iter < 1 or iter > 1000 or np.floor(iter) != iter):
            print("Invalid maximum Iteration!")
            return False
        f.close()
        return True
    except IOError: 
        print("An Error Has Occured")
        return False


def init_datapoints(K, filename):
    centroids = []
    datapoints = []
    i = 0
    with open(filename) as my_file:
        for line in my_file:
            coords = [float(x) for x in line.strip().split(",")]
            if (i < K):
                centroids.append(coords)
                i += 1
            datapoints.append(coords)
    return (centroids, datapoints)

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

def update_centroids(centroids, cent_to_dots_map, datapoints):
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

def euclid_dist(vector1, vector2):
    point1 = np.array(vector1)
    point2 = np.array(vector2)
    return np.linalg.norm(point1 - point2)

def run_kmeans(K, filename, iter=200):
    eps = 0.001
    centroids, datapoints = init_datapoints(K, filename)
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
        update_centroids(centroids, cent_to_dots_map, datapoints)
        conv_flag = convergence(centroids, prev, eps)
        j = j + 1
        print_centroids(centroids)
        print("\n")
    #print_centroids(centroids)

cont = True
if (len(sys.argv) == 4):
    K, iter, filename = sys.argv[1:]
elif (len(sys.argv) == 3):
    K, filename = sys.argv[1:]
    iter = 200
else:
    print("An Error Has Occured")
    cont = False

if (cont):
    K = float(K)
    iter = float(iter)
    if (validate_input(K, iter, filename)):
        run_kmeans(K, filename, iter)