#!/usr/bin/env python3

import numpy as np
import sys

class Centroid:
    def __init__(self, coords):
        self.coords = coords
        self.points = []

    def add_point(self, point):
        self.points.append(point)

    def point_coord_array(self):
        arr = []
        for point in self.points:
            arr.append(point.coords)
        return arr

    def del_points(self):
        self.points = []
    

class DataPoint:
    def __init__(self, coords, centroid):
        self.coords = coords
        self.centroid = centroid


def init_datapoints(K, filename):
    centroids = []
    datapoints = []
    coords = []
    i = 0
    with open(filename) as my_file:
        for line in my_file:
            coords = [float(x) for x in line.strip().split(",")]
            cent = Centroid(coords)
            if (i < K):
                centroids.append(cent)
            datapoints.append(DataPoint(coords, cent))
            coords = []
            i += 1
    return (centroids, datapoints)


def assign_to_cluster(vec_xi, i, centroids):
    min_dist = float('inf')
    min_cent = 0
    for j, vec_cent in enumerate(centroids):
        dist = euclid_dist(vec_xi.coords, vec_cent.coords)
        if (dist <= min_dist):
            min_dist = dist
            min_cent = j
    vec_xi.centroid = centroids[min_cent]
    centroids[min_cent].add_point(vec_xi)
    
    
def update_centroids(centroids):
    for cent in centroids:
        all_coords = np.array(cent.point_coord_array())
        mu_k = np.mean(all_coords, axis=0)
        cent.coords = mu_k

def convergence(centroids, prev, eps):
    print("see prev:")
    for cent2 in prev:
        print(",".join(str(x) for x in cent2.coords))
    print("see updated centroids:")
    for cent in centroids:
            print(",".join(str(x) for x in cent.coords))
    for cent1, cent2 in zip(centroids, prev):
        print("cent1 coords", cent1.coords, "cent2 coords:", cent2.coords)
        delta_mu = euclid_dist(cent1.coords, cent2.coords)
        # print("delta_mu", delta_mu)
        if (delta_mu >= eps):
            return False
    return True

def euclid_dist(vector1, vector2):
    point1 = np.array(vector1)
    point2 = np.array(vector2)
    return np.linalg.norm(point1 - point2)

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


def empty_cents_pts(centroids):
    for cent in centroids:
        cent.del_points()

def run_kmeans(K, filename, iter=200):
    eps = 0.001
    centroids, datapoints = init_datapoints(K, filename)
    conv_flag = False
    j = 0
    while ((not conv_flag) and (j < iter)):
        prev = centroids.copy()
        empty_cents_pts(centroids)
        for i, vec_xi in enumerate(datapoints):
            assign_to_cluster(vec_xi, i, centroids)
        update_centroids(centroids)
        #print("iter:", j)
        conv_flag = convergence(centroids, prev, eps)
        #print("flag: ", conv_flag)
        j = j + 1

    print("j", j)
    for cent in centroids:
       print(",".join(str(x) for x in cent.coords))

K, iter, filename = sys.argv[1:]
K = float(K)
iter = float(K)
if (validate_input(K, iter, filename)):
    run_kmeans(K, filename, iter)