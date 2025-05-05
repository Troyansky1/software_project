# ifndef SYMNMF_H_
# define SYMNMF_H_
# include "lin_alg_utils.h"
# include "utils.h"

# define eps 0.0001
# define MAX_ITER 300

int check_matrix_inits(double** HT, double** H_HT, double** H_HT_H, double** H_next);

double** update_H(double** H, double** W, int n, int k);

double** run_symnmf(double** W, double** H, int k, int n, int print);

void run_sym(double **X, int n, int d);

void run_ddg(double **X, int n, int d);

double** run_norm(double **W, double **X, int n, int d, int print);

void run_goal(char* goal, double** X, int n, int d);

double* derive_clustering_sol(double** H, int n, int k);

# endif