# ifndef SYMNMF_H_
# define SYMNMF_H_
# include "lin_alg_utils.h"
# include "utils.h"

# define eps 0.0001
# define MAX_ITER 300

float** update_H(float** H, float** W, int n, int k);

int check_convergence(float** H, float** H_next, int n, int k);

float** run_symnmf(float** W, float** H, int k, int n, int print);

void run_sym(float **X, int n, int d);

void run_ddg(float **X, int n, int d);

float** run_norm(float **W, float **X, int n, int d, int print);

void run_goal(char* goal, float** X, int n, int d);

float* derive_clustering_sol(float** H, int n, int k);

# endif