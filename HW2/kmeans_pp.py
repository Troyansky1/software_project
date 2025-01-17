#!/usr/bin/env python3

import math
import sys
import numpy as np
import pandas as pd
import mykmeans as mk

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
        

def validate_input(K, iter, N):
    if (K <= 1 or K >= N or K != int(K)):
        print("Invalid number of clusters!")
        return False        
    if (iter <= 1 or iter >= 1000 or iter != int(iter)):
        print("Invalid maximum iteration!")
        return False    
    return True


def init_centroids(data_points, K):
    # each row is a centroid, each column is index of coordinate
    data_points = data_points.reset_index(drop=True)
    centroids = data_points.iloc[:K].copy()
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

def run_kmeans(K, data_points, eps, iter=200):
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
        for i, vec_xi in enumerate(data_points):
            assign_to_cluster(vec_xi, i, centroids, cent_to_dots_map, dot_to_cent_map)
        update_centroids(centroids, cent_to_dots_map)
        conv_flag = convergence(centroids, prev, eps)
        j = j + 1
    print_centroids(centroids)

def main(args):    
    if (len(args) == 5):
        K, eps, filename1, filename2 = args[1:]
        iter = 300
    elif (len(args) == 6):
        K, iter, eps, filename1, filename2 = args[1:]
        iter = int(iter, base=10)
    else:
        print("An Error Has Occurred")
        return
    K = int(K, base=10)
    eps = float(eps)
    print(f"Filename 1: {filename1}, Filename 2:  {filename2}")
    dps1 = pd.read_csv(filename1, header=None)
    dps2 = pd.read_csv(filename2, header=None)
    data_points = pd.merge(dps1, dps2, how='inner', on=0)
    
    data_points = data_points.sort_values(by=0,ascending=True)
    #Print first line
    dim = len(data_points.iloc[0]) -1
    data_points = data_points.iloc[:,1:]
    N = len(data_points)    
    print(f"Num points = {N}, The dimension is {dim}, the number of iterations is: {iter}, epsilon = {eps}, K = {K}")
    # print(data_points.sort_values(by=0,ascending=True))
    # print(data_points.head(K))
    # print(data_points.info())
    centroids = init_centroids(data_points, K)
    #if (centroids == None or data_points == None):
    #    return
    if (validate_input(K, iter, N)):
        cents = mk.fit(centroids, data_points, iter, N, K, dim)
        #print_centroids(mk.fit(centroids, data_points, iter, N, K, dim))


main(sys.argv)

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
