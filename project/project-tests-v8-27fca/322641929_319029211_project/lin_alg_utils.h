# ifndef LIN_ALG_UTILS_H_
# define LIN_ALG_UTILS_H_
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "utils.h"

int check_convergence(double** H, double** H_next, int n, int k);

double calc_euclid_dist(double *a, double *b, int d);

double calc_similarity(double *a, double *b, int d);

double** calc_similarity_matrix(double **X, int n, int d);

double* calc_diag_deg_vec(double **A, int n);

void calc_inv_sqrt(double *D, int n);

void calc_norm_sim_matrix(double **W, double **A, double *D, int n);

double** transpose(double **H, int n, int k);

void mat_mult(int a_rows, int a_cols, int b_cols, double** mat_a, double** mat_b, double** prod);

double inner_prod(double* vec_a, double *vec_b, int dim);

int compute_intermediate_matrices(double** H, int n, int k, double** HT, double** H_HT, double** H_HT_H) ;

double** mat_sub( int dim1, int dim2, double** mat_a, double** mat_b);

double calc_frob_norm(double** H, double** H_next, int n, int k);

# endif