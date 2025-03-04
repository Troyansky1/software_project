# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "symnmf.h"

#define e 2.71828
#define eps e-4
#define MAX_ITER 300

float* init_vec_mem(int n) {
    float *p;
    /* Allocate contiguous memory for the vector */
    p = calloc(n, sizeof(float));  
    if (!p){
        printf("An Error Has Occurred\n");
        return NULL;  
    }
    return p;
}

float** init_matrix_mem(int dim1, int dim2) {
    float *p;
    float **a;
    int i;
    /* Allocate contiguous memory for the matrix */
    p = calloc(dim1 * dim2, sizeof(float));  
    if (!p){
        printf("An Error Has Occurred\n");
        return NULL;  
    } 
    
    a = calloc(dim1, sizeof(float*));  
    if (!a) {
        free(p);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    for (i = 0; i < dim1; i++)
        a[i] = p + i * dim2;

    return a;
}

void free_matrix_mem(float** a) {
    if (a) {
        /* Free the contiguous block of memory */
        free(a[0]);  
        /*  Free the row pointers */
        free(a);     
    }
}

void get_matrix_params(int *n, int *d, FILE* file) {
    char ch;
    int first_row = 1;
    *n = 0;
    *d = 1; 
    while ((ch = fgetc(file)) != EOF) {
        if (first_row) {
            if (ch == ',') {
                (*d)++; 
            }
        }
        if (ch == '\n') {
            (*n)++; 
            first_row = 0;
        }
    }
    /* handle case of file not ending with new line
    if (ch != '\n' && d > 0) {
        n++;
    } */
    printf("The num of lines is: %d\n", *n);
    printf("The num of rows is: %d\n", *d);
    rewind(file);
}

void init_X(FILE* file, float** X, int n){
    /* Initialize X with values*/
    int i;
    int j;
    /* Buffer to hold each line TODO handle bigger lengths*/
    char line[1024];  
    char *ptr;  
    for (i = 0; i < n; i++){
        fgets(line, sizeof(line), file);
        ptr = line;
        j = 0;
        /*  Parse the comma-separated values in the line */
        while (*ptr != '\0') {
            if (sscanf(ptr, "%f", &X[i][j]) == 1){
                while (*ptr != ',' && *ptr != '\0') {
                    ptr++;
                }
                if (*ptr == ',') {
                    ptr++; 
                    j++;
                }
            }
            else{
                printf("An Error Has Occurred\n");
                free_matrix_mem(X);
                return; 
            }
        }
    }
}


float** create_X(FILE* file){
    int n;
    int d;
    float** X;
    get_matrix_params(&n, &d, file);
    if (n <= 0 || d <= 0){
        printf("An Error Has Occurred\n");
        return NULL; 
    }
    X = init_matrix_mem(n, d);
    init_X(file, X, n);
    return X;
}

float calc_euclid_dist(float *a, float *b, int d){
    int i;
    float dist = 0;
    float tmp = 0;
    for (i = 0; i < d; i++){
        tmp = a[i] - b[i];
        tmp = tmp*tmp;
        dist += tmp;
    }
    return dist;
}

float calc_similarity(float *a, float *b, int d){
    float dist = calc_euclid_dist(a, b, d);
    float value;
    value = exp(-0.5 * dist);
    return value;
}

float** calc_similarity_matrix(float **X, int n, int d){
    int i;
    int j;
    float ** A;
    A = init_matrix_mem(n, n);
    if (A == NULL) return 0;
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            if (i == j){
                A[i][j] = 0;
            }
            else{
                A[i][j] = calc_similarity(X[i], X[j], d);
            }
        }
    }
    return A;
}

float* calc_diag_deg_vec(float **A, int n){
    int i;
    int j;
    float d_i;
    float *D = init_vec_mem(n);
    for (i = 0; i < n; i++){
        d_i = 0;
        for (j = 0; j < n; j++){
            d_i += A[i][j];
        }
        D[i] = d_i;
    }
    return D;
}

void calc_inv_sqrt(float *D, int n){
    int i;
    for (i = 0; i < n; i++){
        D[i] = 1/(sqrt(D[i]));
    }
}


float** calc_norm_sim_matrix(float **A, float *D, int n){
    int i;
    int j;
    float** W = init_matrix_mem(n, n);
    calc_inv_sqrt(D, n);
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            W[i][j] = D[i]*A[i][j]*D[j];
        }
    }
    return W;
}

float calc_m(float **W, int n){
    int i;
    int j;
    float avg = 0;
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            avg += W[i][j];
        }
    }
    avg = avg/(n*n);
    return avg;
}

float random_float_in_range(float min, float max) {
    return (rand() / (float)RAND_MAX) * (max - min) + min;  
}

float** init_H(float **W, int k, int n){
    float m = calc_m(W, n);
    float** H = init_matrix_mem(n, k);
    int i;
    int j;
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            H[i][j] = random_float_in_range(0, 2*sqrt(m/k));
        }
    }
    return H;
}

float** transpose(float **H, int n, int k){
    float** H_T = init_matrix_mem(k, n);
    int i;
    int j;
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            H_T[i][j] = H[j][i];
        }
    }
    return H_T;
}

float** mat_mult(int a_rows, int a_cols, int b_cols, float** mat_a, float** mat_b){
    float** prod;
    int i, j, l;
    float sum;
    prod = init_matrix_mem(a_rows, b_cols);
    for (i = 0; i < a_rows; i++){
        for (j = 0; j < b_cols; j++){
            sum = 0;
            for (l = 0; l < a_cols; l++){
                sum += mat_a[i][l] * mat_b[l][j];
            }
            prod[i][j] = sum;
        }            
    }
    return prod;
}

float inner_prod(float* vec_a, float *vec_b, int dim){
    float prod;
    int i;
    prod = 0;
    for (i = 0; i < dim; i++){
        prod+= vec_a[i] * vec_b[i];
    }
    return prod;
}

float** update_H(float** H, float** W, int n, int k){
    float** HT = transpose(H, n, k);
    float beta = 0.5;
    float W_H_ij;
    float** H_HT;
    float** H_HT_H;
    int i;
    int j;
    float** H_next;
    H_next = init_matrix_mem(n, k);
    H_HT = mat_mult(n, k, n, H, HT);
    H_HT_H = mat_mult(n, n, k, H_HT, H);
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            W_H_ij = inner_prod(W[i], HT[j], k);
            H_next[i][j] = H[i][j]*(1 - beta + beta*(W_H_ij/H_HT_H[i][j]));
        }
    }
    /* free memory*/
    return H_next;
}

float** mat_sub( int dim1, int dim2, float** mat_a, float** mat_b){
    int i;
    int j;
    float** sub;
    sub = init_matrix_mem(dim1, dim2);
    for (i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            sub[i][j] = mat_a[i][j] - mat_b[i][j];
        }
    }
    return sub;
}

float calc_frob_norm(float** H, float** H_next, int n, int k){
    float** sub = mat_sub(n, k, H_next, H);
    int i;
    int j;
    float norm = 0;
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            norm += pow(sub[i][j], 2);
        }
    }
    norm = sqrt(norm);
    return norm;
}

int check_convergence(float** H, float**H_next, int n, int k){
    float norm = calc_frob_norm(H, H_next, n, k);
    if (norm < eps){
        return 1;
    }
    return 0;
}

float** optimize_H(float** W, int k, int n){
    float** H; 
    float** H_next; 
    int convergence = 0;
    int i = 0;
    H = init_H(W, k, n);    
    while (!convergence && i <= MAX_ITER){        
        H_next = update_H(H, W, n, k);
        convergence = check_convergence(H, H_next, n, k);
        i++;
    }
    return H_next;
}

void derive_clustering_sol();

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1],"r");
    if (argc != 2){
        printf("An Error Has Occurred\n");
        return 0;
    }
    if (fp == NULL){
        printf("An Error Has Occurred\n");
        return 0;
    }
    create_X(fp);
    fclose(fp);
    return 1;
}