# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

float** update_H(float** H, float** W, int n, int k) {
    /*
    * Updates matrix H using matrix factorization techniques.
    * Parameters:
    *   - H: Pointer to matrix H (size n x k).
    *   - W: Pointer to matrix W (size n x k).
    *   - n: Number of rows in H and W.
    *   - k: Number of columns in H and W.
    * Returns:
    *   - Pointer to the updated matrix H_next (size n x k).
    */
    int i, j;
    float beta = 0.5, W_H_ij;
    float **HT, **H_HT, **H_HT_H, **H_next;
    HT = transpose(H, n, k);
    /*print_matrix(HT, k, n);*/
    if (HT == NULL) return 0;
    H_HT = init_matrix_mem(n, n);
    if (H_HT == NULL) {
        free_matrix_mem(HT);
        return NULL;
    }
    H_HT_H = init_matrix_mem(n, k);
    if (H_HT_H == NULL) {
        free_matrix_mem(HT);
        free_matrix_mem(H_HT);
        return NULL;
    }
    if (!compute_intermediate_matrices(H, n, k, HT, H_HT, H_HT_H)) return NULL;
    H_next = init_matrix_mem(n, k);
    if (H_next == NULL) {
        free_matrix_mem(HT);
        free_matrix_mem(H_HT);
        free_matrix_mem(H_HT_H);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            W_H_ij = inner_prod(W[i], HT[j], n);
            /* update H(t) using the given rule */         
            H_next[i][j] = H[i][j] * (1 - beta + beta * (W_H_ij / H_HT_H[i][j]));
        }
    }
    free_matrix_mem(HT);
    free_matrix_mem(H_HT);
    free_matrix_mem(H_HT_H);
    return H_next;
}

int check_convergence(float** H, float** H_next, int n, int k){
    /*
    * Checks if the convergence condition has been met by comparing the Frobenius norm 
    * of the difference between two matrices H and H_next to a predefined threshold eps.
    * Parameters:
    *   - H: Pointer to the matrix H (size n x k).
    *   - H_next: Pointer to the matrix H_next (size n x k).
    *   - n: The number of rows in both matrices.
    *   - k: The number of columns in both matrices.
    * Returns:
    *   - 1 if convergence is reached (i.e., norm < eps).
    *   - 0 if convergence is not reached.
    *   - -1 if there was an error.
    */
    float norm = calc_frob_norm(H, H_next, n, k);
    if (norm < 0){
        return -1;
    }
    if (norm < eps){
        return 1;
    }
    return 0;
}

float** run_symnmf(float** W, float** H, int k, int n, int print){
    /*
    * Performs the symmetric non-negative matrix factorization algorithm 
    * Iterates until convergence is smaller than epsilon or until it reaches the max number of iterations.
    * Parameters:
    *   - W: Pointer to the matrix W (n x k).
    *   - H: Pointer to the matrix H (n x k).
    *   - k: The number of columns in matrix H (and matrix W).
    *   - n: The number of rows in matrix H (and matrix W).
    *   - print: A boolean indicating whether to print the matrix(1) or not(0)
    * Returns:
    *   - Pointer to the updated matrix H_next (n x k).
    */
    float **H_next; 
    int convergence = 0;
    int i = 0;  
    while (!convergence && i <= MAX_ITER){  
        H_next = update_H(H, W, n, k);
        if (H_next == NULL){
            free_matrix_mem(W);
            free_matrix_mem(H);
            printf("An Error Has Occurred\n");
            return NULL;
        }
        convergence = check_convergence(H, H_next, n, k);
        if (convergence == -1){
            free_matrix_mem(W);
            free_matrix_mem(H);
            free_matrix_mem(H_next);
            printf("An Error Has Occurred\n");
            return NULL;        
        }
        i++;
        free_matrix_mem(H);
        H = H_next;
    }
    if (print) {print_matrix(H_next, n, k);}
    return H_next;
}

void run_sym(float **X, int n, int d){
    /*
    * Computes the similarity matrix A from the input matrix X and prints the resulting matrix A. 
    * Parameters:
    *   - X: Pointer to the matrix X (n x d).
    *   - n: The number of data points (number of rows in the matrix X and number of rows and columns in matrix A).
    *   - d: The number of columns in the matrix X.
    * Returns:
    *   - None.
    */
    float ** A;
    A = calc_similarity_matrix(X, n, d);
    if (A == NULL) {
        printf("An Error Has Occurred\n");
        return;
    }
    print_matrix(A, n, n);
    free_matrix_mem(A);
}

void run_ddg(float **X, int n, int d){
    /*
    * Computes the diagonal degree matrix D from the similarity matrix A, 
    * which is derived from the input matrix X. prints D at the end.
    * Parameters:
    *   - X: Pointer to the matrix X (n x d).
    *   - n: The number of data points (rows in matrix X and the size of the diagonal matrix D).
    *   - d: The number of columns in matrix X.
    * Returns:
    *   - None.
    */
    float **A;
    float *D;
    A = calc_similarity_matrix(X, n, d);
    if (A == NULL) {
        printf("An Error Has Occurred\n");
        return;
    }
    D = calc_diag_deg_vec(A, n);
    if (D == NULL) {
        free_matrix_mem(A);
        printf("An Error Has Occurred\n");
        return;
    }
    print_diag_matrix(D, n);
    free_matrix_mem(A);
    free(D);
}

float** run_norm(float **W, float **X, int n, int d, int print){
    /*
 * Computes the normalized similarity matrix W from the input matrix X and prints it.
 * Parameters:
    *   - X: Pointer to the matrix X (n x d).
    *   - n: The number of data points (rows in matrix X and the size of the diagonal matrix D).
    *   - d: The number of columns in matrix X.
 * Returns:
 *   - None.
 */
    float *D;
    float **A;
    A = calc_similarity_matrix(X, n, d);
    if (A == NULL) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    D = calc_diag_deg_vec(A, n);
    if (D == NULL) {
        free_matrix_mem(A);
        printf("An Error Has Occurred\n");
        return NULL;
    } 
    calc_norm_sim_matrix(W, A, D, n);
    if (W == NULL) {
        free_matrix_mem(A);
        free(D);
        printf("An Error Has Occurred\n");
        return NULL;
    } 
    if (print == 1){
        print_matrix(W, n, n); 
    }
    free_matrix_mem(A);
    free(D);

    return W;
}

void run_goal(char* goal, float** X, int n, int d){
    /*
    * Executes a specific function based on the provided goal string. The function checks 
    * the value of goal and calls the corresponding function: sym, ddg, or norm. 
    * If the goal is not recognized, an error message is printed.
    * Parameters:
    *   - goal: A string indicating the goal to execute. It can be "sym", "ddg", or "norm".
    *   - X: Pointer to the matrix X (n x d).
    *   - n: The number of data points (rows in matrix X and the size of the diagonal matrix D).
    *   - d: The number of columns in matrix X.
    * Returns:
    *   - None.
    */
    if(strcmp(goal, "sym") == 0){
        run_sym(X, n ,d);
    }
    else if(strcmp(goal, "ddg") == 0){
        run_ddg(X, n, d);
    }
    else if (strcmp(goal, "norm") == 0){
        float** W = init_matrix_mem(n, n);
        if (W == NULL){
            printf("An Error Has Occurred\n");
            return;
        }
        run_norm(W, X, n, d, 1);
        free_matrix_mem(W);
    }
    else{
        printf("An Error Has Occurred\n");
    }
}

float* derive_clustering_sol(float** H, int n, int k){
    int i, j;
    float max;
    float* hard_clustering = init_vec_mem(n);
    for (i = 0; i < n; i++){
        max = 0;
        for (j = 0; j < k; j++){
            if (H[i][j] > max){
                max = H[i][j];
                hard_clustering[i] = j;
            }
        }
    }
    return hard_clustering;
}

int main(int argc, char **argv){
    float** X;
    char* goal;
    int n, d;
    FILE *fp;
    if (argc != 3){
        printf("An Error Has Occurred\n");
        return 1;
    }
    fp = fopen(argv[2],"r");
    if (fp == NULL){
        printf("An Error Has Occurred\n");
        return 1;
    }
    get_matrix_params(&n, &d, fp);
    if (n <= 0 || d <= 0){
        printf("An Error Has Occurred\n");
        return 1; 
    }
    X = create_X(fp, n, d);
    if (X == NULL){
        printf("An Error Has Occurred\n");
        return 1; 
    }
    fclose(fp);
    goal = argv[1];
    run_goal(goal, X, n, d);
    free_matrix_mem(X);
    return 0;
}