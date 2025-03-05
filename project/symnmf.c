# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

#define e 2.71828
#define eps e-4
#define MAX_ITER 300


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
    for(i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            printf("%.4f", matrix[i][j]);
            if (j < dim2 -1) printf("%c", ',');
            else printf("%c", '\n');
        }
    }
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


void init_Xold(FILE* file, float** X, int n){
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
        prod+= vec_a[i] * vec_b[i];
    }
    return prod;
}

float** update_H(float** H, float** W, int n, int k){
    /*
    * Updates the matrix H based on matrix factorization techniques using the 
    * matrix W and intermediate calculations. The updated matrix H_next is 
    * computed by first transposing H, performing matrix multiplications, 
    * and then updating each element of H based on a formula involving the 
    * inner product of W and HT.
    * Parameters:
    *   - H: Pointer to the matrix H (size n x k).
    *   - W: Pointer to the matrix W (size n x k).
    *   - n: The number of rows in matrix H and W.
    *   - k: The number of columns in matrix H and W.
    * Returns:
    *   - Pointer to the updated matrix H_next (n x k).
    */
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
    */
    float norm = calc_frob_norm(H, H_next, n, k);
    if (norm < eps){
        return 1;
    }
    return 0;
}

float** symnmf(float** W, float** H, int k, int n){
    /*
    * Performs the symmetric non-negative matrix factorization algorithm 
    * Iterates until convergence is smaller than epsilon or until it reaches the max number of iterations.
    * Parameters:
    *   - W: Pointer to the matrix W (n x k).
    *   - H: Pointer to the matrix H (n x k).
    *   - k: The number of columns in matrix H (and matrix W).
    *   - n: The number of rows in matrix H (and matrix W).
    * Returns:
    *   - Pointer to the updated matrix H_next (n x k).
    */
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
    print_matrix(A, n, n);
}

void ddg(float **X, int n, int d){
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
    D = calc_diag_deg_vec(A, n);
    print_diag_matrix(D, n);
}

void norm(float **X, int n, int d){
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
    float **W, **A;
    A = calc_similarity_matrix(X, n, d);
    D = calc_diag_deg_vec(A, n);    
    W = calc_norm_sim_matrix(A, D, n);
    print_matrix(W, n, n);
}

void derive_clustering_sol();

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