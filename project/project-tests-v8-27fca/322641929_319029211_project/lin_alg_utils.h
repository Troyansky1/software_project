# ifndef LIN_ALG_UTILS_H_
# define LIN_ALG_UTILS_H_
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "utils.h"

float calc_euclid_dist(float *a, float *b, int d);

float calc_similarity(float *a, float *b, int d);

float** calc_similarity_matrix(float **X, int n, int d);

float* calc_diag_deg_vec(float **A, int n);

void calc_inv_sqrt(float *D, int n);

void calc_norm_sim_matrix(float **W, float **A, float *D, int n);

float** transpose(float **H, int n, int k);

void mat_mult(int a_rows, int a_cols, int b_cols, float** mat_a, float** mat_b, float** prod);

float inner_prod(float* vec_a, float *vec_b, int dim);

int compute_intermediate_matrices(float** H, int n, int k, float** HT, float** H_HT, float** H_HT_H) ;

float** mat_sub( int dim1, int dim2, float** mat_a, float** mat_b);

float calc_frob_norm(float** H, float** H_next, int n, int k);

# endif