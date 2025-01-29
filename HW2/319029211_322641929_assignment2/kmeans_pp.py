#!/usr/bin/env python3

import math
import sys
import numpy as np
import pandas as pd
import mykmeanssp as mk

np.random.seed(1234)

def print_centroids(centroids):
    for cent in centroids:
        exp = ""
        for c in cent:
            exp += '{:.4f}'.format(c) + ","
        print(exp[:-1])
    print()


def validate_input(K, iter, N):
    if (K <= 1 or K >= N or K != int(K)):
        print("Invalid number of clusters!")
        return False        
    if (iter <= 1 or iter >= 1000 or iter != int(iter)):
        print("Invalid maximum iteration!")
        return False    
    return True

def euclid_dist(vector1, vector2):
    squared_differences = [(float(v1) - float(v2)) ** 2 for v1, v2 in zip(vector1, vector2)]
    sum_of_squares = sum(squared_differences)
    norm = math.sqrt(sum_of_squares)
    return norm

def find_nearest_center_dist(centroids, data_point):
    min_dist = np.inf
    for k in range(len(centroids)):        
        dist = euclid_dist(centroids.iloc[k].values, data_point.values) 
        if dist < min_dist:
            min_dist = dist
    return min_dist

def get_weights(centroids, data_points):
    weights = []
    for i in range(len(data_points)):
        weights.append(find_nearest_center_dist(centroids, data_points.iloc[i]))
    sum_weights = sum(weights)
    norm_weights = [x/sum_weights for x in weights]
    return pd.Series(data=norm_weights, copy=False)

def init_centroids(data_points, K):
    # each row is a centroid, each column is index of coordinate
    #data_points = data_points.reset_index(drop=True)
    centroids = pd.DataFrame()    
    i = np.random.choice(len(data_points))    
    indices = [i]
    centroids = pd.concat([centroids,data_points.iloc[[i]]], ignore_index=True)
    for k in range(K -1):
        weight = get_weights(centroids, data_points)
        j = np.random.choice(len(data_points), p=weight) 
        centroids = pd.concat([centroids, data_points.iloc[[j]]], ignore_index=True)
        indices.append(j)
    return centroids, indices


def main(args):    
    if (len(args) == 5):
        K, eps, filename1, filename2 = args[1:]
        iter = 300
    elif (len(args) == 6):
        K, iter, eps, filename1, filename2 = args[1:]
        try:
            iter = int(iter, base=10)
        except ValueError:
            print("Invalid maximum iteration!")
            return 
    else:
        print("An Error Has Occurred")
        return
    try:
        K = int(K, base=10)
    except ValueError:
        print("Invalid number of clusters!")
        return 
    eps = float(eps)
    try:
        dps1 = pd.read_csv(filename1, header=None)
        dps2 = pd.read_csv(filename2, header=None)
    except IOError:
        print("An Error Has Occurred")
        return
    data_points = pd.merge(dps1, dps2, how='inner', on=0)
    data_points = data_points.sort_values(by=0,ascending=True)
    dim = len(data_points.iloc[0]) -1
    data_points = data_points.iloc[:,1:]
    N = len(data_points)    
    centroids, indices = init_centroids(data_points, K)
    if (validate_input(K, iter, N)):   
        data_points = data_points.values.tolist()
        centroids = centroids.values.tolist()  
        print(",".join(map(str, indices)))
        ret_centroids = mk.fit(centroids, data_points, iter, N, K, dim, eps)


main(sys.argv)

