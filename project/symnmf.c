# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

#define eps 0.0001
#define MAX_ITER 300

/* Might be worth putting all the prints, calcs and so on in a seperate file */

void print_matrix(float** matrix, int dim1, int dim2){
    /*
    * Prints a matrix with dimensions dim1 x dim2. Each element of the 
    * matrix is printed with a precision of four decimal places, with commas separating 
    * elements in a row and newlines between rows.
    * Parameters:
    *   - matrix: Pointer to the matrix (dim1 x dim2) to be printed.
    *   - dim1: The number of rows in the matrix.
    *   - dim2: The number of columns in the matrix.
    * Returns:
    *   - None.
    */
    int i, j;
    printf("------------------\n");
    for(i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            printf("%.8f", matrix[i][j]);
            if (j < dim2 -1) printf("%c", ',');
            else printf("%c", '\n');
        }
    }
    printf("------------------\n");
}

void print_diag_matrix(float* vector, int dim){
    /*
    * Prints the diagonal matrix representation of a given vector. The function takes a 
    * vector of size dim and prints it as a diagonal matrix, where the diagonal entries 
    * correspond to the elements of the vector and all off-diagonal entries are zero.
    * Each element of the matrix is printed with a precision of four decimal places, with commas separating 
    * elements in a row and newlines between rows.
    * Parameters:
    *   - vector: Pointer to the vector (size dim) to be printed as a diagonal matrix.
    *   - dim: The dimension of the vector (and the size of the resulting square matrix).
    * Returns:
    *   - None.
    */
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

void get_matrix_params(int *n, int *d, FILE* file){
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


char* read_line(FILE* file, size_t* buffer_size) {
    /*
    * Reads a line from a file and dynamically resizes memory if needed.
    * Parameters:
    *   - file: Open file pointer to read from.
    *   - buffer_size: Pointer to the initial buffer size, updated if resized.
    * Returns:
    *   - Pointer to the dynamically allocated line (caller must free).
    *   - NULL on allocation failure.
    */
    size_t len = 0;
    char *line = (char*)malloc(*buffer_size * sizeof(char));
    char *temp;
    if (line == NULL) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    while (fgets(line + len, (int)(*buffer_size - len), file) != NULL) {
        len = strlen(line);
        if (line[len - 1] == '\n') break;
        *buffer_size *= 2;
        temp = (char*)realloc(line, *buffer_size);
        if (temp == NULL) {
            printf("An Error Has Occurred\n");
            free(line);
            return NULL;
        }
        line = temp;
    }
    return line;
}


int parse_line_to_array(char *line, float *row, int d) {
    /*
    * Parses a comma-separated string into an array of floats.
    * Parameters:
    *   - line: Input string containing comma-separated values.
    *   - row: Array to store parsed float values.
    *   - d: Expected number of values (columns).
    * Returns:
    *   - 1 if parsing is successful, 0 if an error occurs.
    */
    char *ptr = line;
    int j = 0;
    while (*ptr != '\0' && *ptr != '\n' && j < d) {
        if (sscanf(ptr, "%f", &row[j]) == 1) {
            while (*ptr != ',' && *ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == ',') ptr++;
            j++;
        } else {
            return 0; 
        }
    }
    return (j == d);
}


void init_X(FILE* file, float** X, int n, int d) {
    /*
    * Reads matrix values from a text file into X.
    * Parameters:
    *   - file: Open file pointer to read from.
    *   - X: 2D float array (size n x d) to store parsed values.
    *   - n: Number of rows in X.
    *   - d: Number of columns in X.
    * Returns:
    *   - None (modifies X in place). Prints an error and frees X on failure.
    */
    int i;
    size_t buffer_size = 128;
    char *line;

    for (i = 0; i < n; i++) {
        line = read_line(file, &buffer_size);
        if (line == NULL || !parse_line_to_array(line, X[i], d)) {
            printf("An Error Has Occurred\n");
            free(line);
            free_matrix_mem(X);
            return;
        }
        free(line);
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
    init_X(file, X, n, d);
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
    if (H == NULL) return NULL;
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
    /*
    printf("_______mat mult_______\n");
    print_matrix(prod, a_rows, b_cols);
    printf("\n\n");
    */
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
            W_H_ij = inner_prod(W[i], H[j], k);
            /* update H(t) using the given rule */         
            H_next[i][j] = H[i][j] * (1 - beta + beta * (W_H_ij / H_HT_H[i][j]));
        }
    }
    free_matrix_mem(HT);
    free_matrix_mem(H_HT);
    free_matrix_mem(H_HT_H);
    return H_next;
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
    if (print) print_matrix(H_next, n, k);
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

float** run_norm(float **W, float **X, int n, int d){
    /*
 * Computes the normalized similarity matrix W from the input matrix X and prints it.
 * Parameters:
    *   - X: Pointer to the matrix X (n x d).
    *   - n: The number of data points (rows in matrix X and the size of the diagonal matrix D).
    *   - d: The number of columns in matrix X.
 * Returns:
 *   - None.
 */
    float * D;
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
    /* print_matrix(W, n, n); */
    free_matrix_mem(A);
    free(D);
    return W;
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
        run_norm(W, X, n, d);
    }
    else{
        printf("An Error Has Occurred\n");
        printf("Wrong goal name\n");
    }
}

int main(int argc, char **argv){
    float** X;
    char* goal;
    int n, d;
    FILE *fp;
    if (argc != 3){
        printf("An Error Has Occurred\n");
        printf("Wrong num of arguments\n");
        return 0;
    }
    fp = fopen(argv[2],"r");
    if (fp == NULL){
        printf("An Error Has Occurred\n");
        printf("can't open file\n");
        return 0;
    }
    get_matrix_params(&n, &d, fp);
    if (n <= 0 || d <= 0){
        printf("An Error Has Occurred\n");
        printf("n <= 0 || d <= 0\n");
        return 0; 
    }
    X = create_X(fp, n, d);
    fclose(fp);
    goal = argv[1];
    run_goal(goal, X, n, d);
    free_matrix_mem(X);
    return 1;
}