# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

#define e 2.71828
#define eps e-4
#define MAX_ITER 300


void print_matrix(float** matrix, int dim1, int dim2){
    int i, j;
    for(i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            printf("%.4f", matrix[i][j]);
            if (j < dim2 -1) printf("%c", ',');
            else printf("%c", '\n');
        }
    }
}

void print_diag_matrix(float* vector, int dim){
    int i, j;
    for(i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            if (i == j) printf("%.4f", vector[i]);
            else printf("%d", 0);
            if (j < dim -1) printf("%c", ',');
            else printf("%c", '\n');
        }
    }
}

float* init_vec_mem(int dim) {
    /*
    * Allocates memory for a vector as a contiguous block.
    * Parameters:
    *   - dim: The dimension of the vector.
    * Returns:
    *   - A pointer to an array if successful.
    *   - NULL if memory allocation fails.
    */
    float *p;
    /* Allocate contiguous memory for the vector */
    p = calloc(dim, sizeof(float));  
    if (!p){
        printf("An Error Has Occurred\n");
        return NULL;  
    }
    return p;
}

float** init_matrix_mem(int dim1, int dim2) {
    /*
    * Allocates memory for an dim1 x dim2 matrix as a contiguous block.
    * Parameters:
    *   - dim1: The number of rows.
    *   - dim2: The number of columns.
    * Returns:
    *   - A pointer to an array of row pointers if successful.
    *   - NULL if memory allocation fails.
    */
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

void free_matrix_mem(float** matrix) {
    /*
    * Frees memory allocated for a matrix.
    * Parameters:
    *   - matrix: A pointer to a matrix- an array of row pointers.
    * Returns:
    *   - None.
    */
    if (matrix) {
        /* Free the contiguous block of memory */
        free(matrix[0]);  
        /*  Free the row pointers */
        free(matrix);     
    }
}

void get_matrix_params(int *n, int *d, FILE* file) {
    /*
    * Parses a matrix file to count rows (n) and columns (d).
    * Params:
    *   - n: Pointer to store number of rows.
    *   - d: Pointer to store number of columns.
    *   - file: Open file pointer to read from.
    * 
    * Returns: None (updates *n and *d).
    */
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
    rewind(file);
}

void init_X(FILE* file, float** X, int n){
    /*
    * Reads matrix values from a txt file into X.
    * Params:
    *   - file: Open file pointer to read from.
    *   - X: 2D float array (n x d) to store parsed values.
    *   - n: Number of rows in X.
    * Returns: None (modifies X in place). Prints error and frees X on failure.
    */
    int i, j;
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

float** create_X(FILE* file, int n, int d){
    /*
    * Creates and initializes a matrix X from a txt file.
    * Params:
    *   - file: Open file pointer to read from.
    * 
    * 
    * Returns:
    *   - Pointer to a n x d matrix (float**).
    *   - NULL if an error occurs.
    */
    float** X;    
    X = init_matrix_mem(n, d);
    init_X(file, X, n);
    return X;
}

float calc_euclid_dist(float *a, float *b, int d){
    /*
    * Computes the squared Euclidean distance between two d-dimensional points.
    * Params:
    *   - a: Pointer to the first point (float array).
    *   - b: Pointer to the second point (float array).
    *   - d: Number of dimensions.
    * Returns:
    *   - The squared Euclidean distance between a and b.
    */
    int i;
    float dist = 0;
    float tmp = 0;
    for (i = 0; i < d; i++){
        tmp = a[i] - b[i];
        tmp = pow(tmp, 2);
        dist += tmp;
    }
    return dist;
}

float calc_similarity(float *a, float *b, int d){
    /*
    * Computes the similarity between two d-dimensional points.
    * Params:
    *   - a: Pointer to the first point (float array).
    *   - b: Pointer to the second point (float array).
    *   - d: Number of dimensions.
    * Returns:
    *   - The similarity value between a and b.
    */
    float dist = calc_euclid_dist(a, b, d);
    float value;
    value = exp(-0.5 * dist);
    return value;
}

float** calc_similarity_matrix(float **X, int n, int d){
    /*
    * Computes the similarity matrix for a dataset.
    * Params:
    *   - X: Pointer to the dataset (n x d matrix).
    *   - n: Number of data points (rows).
    *   - d: Number of dimensions (columns).
    * Returns:
    *   - Pointer to the computed n x n similarity matrix.
    *   - NULL if memory allocation fails.
    */
    int i, j;
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
    /*
    * Computes the diagonal degree vector for a similarity matrix.
    * Params:
    *   - A: Pointer to the similarity matrix (n x n).
    *   - n: Number of data points (rows).
    * Returns:
    *   - Pointer to a vector (float array) of size n, representing a diagonal matrix.
    */
    int i, j;
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
        /*
        * Computes D^-(1/2), the inverted sqrt of a n dim diagonal matrix D.
        * Params:
        *   - D: Diagonal matrix, represented by an n sized vector (float array).
        *   - n: Number of data points (rows).
        * Returns:
        *   - Pointer to a vector (float array) of size n, representing the matrix after the transformation.
        */
    int i;
    for (i = 0; i < n; i++){
        if(D[i] != 0){
            D[i] = 1/(sqrt(D[i]));
        }        
    }
}


float** calc_norm_sim_matrix(float **A, float *D, int n){
    /*
    * Computes the normalized similarity matrix
    * Params:
    *   - A: Pointer to the similarity matrix (n * n).
    *   - D: Diagonal matrix, represented by an n sized vector (float array).
    *   - n: Number of data points (rows).
    * Returns:
    *   - Pointer to the computed n x n norm similarity matrix.
    */
    int i, j;
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
    /*
    * Computes the average of all entries of W.
    * Params:
    *   - W: Pointer to the normalized similarity matrix (n * n).
    *   - n: Number of data points (rows).
    * Returns:
    *   - The average (float) of all entries of W.
    */
    int i, j;
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
    /*
    * Returns a random number (float) between min and max.
    * Params:
    *   - min: A number (float) that represents the lower end of the range.
    *   - max: A number (float) that represents the upper end of the range.
    * Returns:
    *   - A random number (float) in the given range.
    */
    return (rand() / (float)RAND_MAX) * (max - min) + min;  
}

float** init_H(float **W, int k, int n){
    /*
    * Randomly initialize H with values from the interval [0, 2 ∗ sqrt(m/k)].
    * Params:
    *   - W: pointer to the normalized similarity matrix (n * n).
    *   - k: Number of clusters (columns).
    *   - n: Number of data points (rows).
    * Returns:
    *   - Pointer to the initialized H matrix.
    */
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
    /*
    * Computes the transposed matrix.
    * Params:
    *   - H: pointer to a matrix (n * k).
    *   - k: Number of clusters (columns).
    *   - n: Number of data points (rows).
    * Returns:
    *   - Pointer to the transposed matrix.
    */
    float **H_T = init_matrix_mem(k, n);
    int i, j;
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            H_T[i][j] = H[j][i];
        }
    }
    return H_T;
}

float** mat_mult(int a_rows, int a_cols, int b_cols, float** mat_a, float** mat_b){
    /*
    * Computes matrix multiplication.
    * Params:
    *   - a_rows: Number of rows in matrix a.
    *   - a_cols: Number of columns in matrix a (equals to the number of rows in matrix b).
    *   - b_cols: Number of rows in matrix a.
    *   - mat_a: Pointer to the first matrix (a_rows * a_cols).
    *   - mat_b: Pointer to the second matrix (a_cols * b_cols). .
    * Returns:
    *   - Pointer to the matrix multiplication.
    */
    float **prod;
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
    int i, j;
    float beta = 0.5;
    float **HT, **H_HT, **H_HT_H, **H_next;
    float W_H_ij;

    HT = transpose(H, n, k);
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
    int i, j;
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
    int i, j;
    float norm = 0;
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            norm += pow(sub[i][j], 2);
        }
    }
    norm = sqrt(norm);
    return norm;
}

int check_convergence(float** H, float** H_next, int n, int k){
    float norm = calc_frob_norm(H, H_next, n, k);
    if (norm < eps){
        return 1;
    }
    return 0;
}

float** symnmf(float** W, float** H, int k, int n){
    float **H_next; 
    int convergence = 0;
    int i = 0;  
    while (!convergence && i <= MAX_ITER){        
        H_next = update_H(H, W, n, k);
        convergence = check_convergence(H, H_next, n, k);
        i++;
    }
    return H_next;
}

void sym(float **X, int n, int d){
    float ** A;
    A = calc_similarity_matrix(X, n, d);
    print_matrix(A, n, n);
}

void ddg(float **X, int n, int d){
    float **A;
    float *D;
    A = calc_similarity_matrix(X, n, d);
    D = calc_diag_deg_vec(A, n);
    print_diag_matrix(D, n);
}

void norm(float **X, int n, int d){
    float * D;
    float **W, **A;
    A = calc_similarity_matrix(X, n, d);
    D = calc_diag_deg_vec(A, n);    
    W = calc_norm_sim_matrix(A, D, n);
    print_matrix(W, n, n);
}

void derive_clustering_sol();

void run_goal(char* goal, float** X, int n, int d){
    if(strcmp(goal, "sym") == 0){
        sym(X, n ,d);
    }
    else if(strcmp(goal, "ddg") == 0){
        ddg(X, n, d);
    }
    else if (strcmp(goal, "norm") == 0){
        norm(X, n, d);
    }
    else{
        printf("An Error Has Occurred\n");
    }
}

int main(int argc, char **argv){
    float** X;
    char* goal;
    int n, d;
    FILE *fp;
    if (argc != 3){
        printf("An Error Has Occurred\n");
        return 0;
    }
    fp = fopen(argv[2],"r");
    if (fp == NULL){
        printf("An Error Has Occurred\n");
        return 0;
    }
    get_matrix_params(&n, &d, fp);
    if (n <= 0 || d <= 0){
        printf("An Error Has Occurred\n");
        return 0; 
    }
    X = create_X(fp, n, d);
    fclose(fp);
    goal = argv[1];
    run_goal(goal, X, n, d);
    return 1;
}