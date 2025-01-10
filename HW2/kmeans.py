#!/usr/bin/env python3

import math
import sys
import numpy as np
import pandas as pd

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
        if (K <= 1 or K >= line_count or K != int(K)):
            print("Invalid number of clusters!")
            return False        
        if (iter <= 1 or iter >= 1000 or iter != int(iter)):
            print("Invalid maximum iteration!")
            return False
        f.close()
        return True
    except IOError: 
        print("An Error Has Occurred")
        return False

def init_centroids(datapoints, K):
    # each row is a centroid, each column is index of coordinate
    centroids = datapoints.iloc[:K, ].copy()
    return centroids


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

def calc_mean(centroid):
    num_points = len(centroid)
    num_coords = len(centroid[0])
    coords_sum = [0] * num_coords

    for coord in centroid:
        for i in range(num_coords):
            coords_sum[i] += coord[i]

    # Calculate the mean for each dimension
    centroid = [coords_sum[i] / num_points for i in range(num_coords)]

    return centroid

def update_centroids(centroids, cent_to_dots_map):
    for i in range(len(centroids)):
        centroids[i] = calc_mean(cent_to_dots_map[i])

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
    squared_differences = [(v1 - v2) ** 2 for v1, v2 in zip(vector1, vector2)]
    sum_of_squares = sum(squared_differences)
    norm = math.sqrt(sum_of_squares)
    return norm

def run_kmeans(K, filename, eps, iter=200):
    datapoints = init_datapoints(K, filename)
    centroids = init_centroids(datapoints, K)
    
    if (centroids == None or datapoints == None):
        return
    
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
    print_centroids(centroids)

    
run_kmeans(3, "/home/developer/software_project-1/HW2/input_1.txt", 0.001, 600)

if (len(sys.argv) == 4):
    K, filename1, filename2, eps = sys.argv[1:]
    iter = 300
elif (len(sys.argv) == 5):
    K, iter, filename1, filename2, eps = sys.argv[1:]
else:
    print("An Error Has Occurred")

dps1 = pd.read_csv(filename1, header=None)
dps2 = pd.read_csv(filename2, header=None)
datapoints = pd.merge(dps1, dps2, how='inner', on='0')
datapoints.sort_values()
init_centroids(datapoints, K)

if (validate_input(K, iter, eps)):
        run_kmeans(K, eps, iter)


"""
cont = True
if (len(sys.argv) == 4):
    K, iter, filename = sys.argv[1:]
elif (len(sys.argv) == 3):
    K, filename = sys.argv[1:]
    iter = 200
else:
    print("An Error Has Occurred")
    cont = False

if (cont):
    K = float(K)
    iter = float(iter)
    if (validate_input(K, iter, filename)):
        run_kmeans(K, filename, iter)
"""