# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

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
    float **A;
    A = init_matrix_mem(n, n);
    if (A == NULL) return NULL;
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
    if (D == NULL) return NULL;
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

void calc_norm_sim_matrix(float **W, float **A, float *D, int n){
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
    calc_inv_sqrt(D, n);
    for (i = 0; i < n; i++){        
        for (j = 0; j < n; j++){   
            W[i][j] = D[i]*A[i][j]*D[j];
        }
    }
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
    int i, j;
    float **H_T = init_matrix_mem(k, n);
    if (H_T == NULL){
        printf("An Error Has Occurred\n");
        return NULL;
    } 
    for (j = 0; j < k; j++){
        for (i = 0; i < n; i++){
            H_T[j][i] = H[i][j];
        }
    }
    return H_T;
}

void mat_mult(int a_rows, int a_cols, int b_cols, float** mat_a, float** mat_b, float** prod){
    /*
    * Computes matrix multiplication.
    * Params:
    *   - a_rows: Number of rows in matrix a.
    *   - a_cols: Number of columns in matrix a (equals to the number of rows in matrix b).
    *   - b_cols: Number of rows in matrix a.
    *   - mat_a: Pointer to the first matrix (a_rows * a_cols).
    *   - mat_b: Pointer to the second matrix (a_cols * b_cols). 
    *   - prod: Pointer to the result matrix (a_rows * b_cols).
    * Returns:
    *   - None
    */
    int i, j, l;
    float sum;
    for (i = 0; i < a_rows; i++){
        for (j = 0; j < b_cols; j++){
            sum = 0;
            for (l = 0; l < a_cols; l++){
                sum += mat_a[i][l] * mat_b[l][j];
            }
            prod[i][j] = sum;            
        }            
    }
}

float inner_prod(float* vec_a, float *vec_b, int dim){
    /*
    * Computes the inner product (dot product) of two vectors vec_a and vec_b of size dim.
    * Parameters:
    *   - vec_a: Pointer to the first vector (size dim).
    *   - vec_b: Pointer to the second vector (size dim).
    *   - dim: The size (dimension) of the vectors.
    * Returns:
    *   - The inner product of the two vectors as a float.
    */
    float prod;
    int i;
    prod = 0;
    for (i = 0; i < dim; i++){
        prod += vec_a[i] * vec_b[i];
    }
    return prod;
}

int compute_intermediate_matrices(float** H, int n, int k, float** HT, float** H_HT, float** H_HT_H) {
    /*
    * Allocates and computes intermediate matrices required for updating H.
    * Parameters:
    *   - H: Pointer to matrix H (size n x k).
    *   - n: Number of rows in H.
    *   - k: Number of columns in H.
    *   - HT: Pointer to store the transposed matrix of H.
    *   - H_HT: Pointer to store H * HT.
    *   - H_HT_H: Pointer to store H_HT * H.
    * Returns:
    *   - 1 if successful, 0 if memory allocation fails.
    */
    
    mat_mult(n, k, n, H, HT, H_HT);
    mat_mult(n, n, k, H_HT, H, H_HT_H);
    return 1;
}

float** mat_sub( int dim1, int dim2, float** mat_a, float** mat_b){
    /*
    * Computes the element-wise subtraction of two matrices mat_a and mat_b.
    * Parameters:
    *   - dim1: The number of rows in both matrices.
    *   - dim2: The number of columns in both matrices.
    *   - mat_a: Pointer to the first matrix (dim1 x dim2).
    *   - mat_b: Pointer to the second matrix (dim1 x dim2).
    * Returns:
    *   - Pointer to the resulting matrix (dim1 x dim2) containing the element-wise differences.
    */
    int i, j;
    float** sub;
    sub = init_matrix_mem(dim1, dim2);
    if (sub == NULL){
        return NULL;
    }
    for (i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            sub[i][j] = mat_a[i][j] - mat_b[i][j];
        }
    }
    return sub;
}

float calc_frob_norm(float** H, float** H_next, int n, int k){
    /*
    * Calculates the Frobenius norm of the difference between two matrices H and H_next.
    * The Frobenius norm is computed as the square root of the sum of the squared element-wise 
    * differences between the two matrices.
    * Parameters:
    *   - H: Pointer to the matrix H (n x k).
    *   - H_next: Pointer to the matrix H_next (n x k).
    *   - n: The number of rows in both matrices.
    *   - k: The number of columns in both matrices.
    * Returns:
    *   - The Frobenius norm as a float.
    */
    int i, j;
    float norm = 0;
    float** sub = mat_sub(n, k, H_next, H);
    if (sub == NULL){
        return -1;
    }
    for (i = 0; i < n; i++){
        for (j = 0; j < k; j++){
            norm += pow(sub[i][j], 2);
        }
    }
    free_matrix_mem(sub);
    return norm;
}