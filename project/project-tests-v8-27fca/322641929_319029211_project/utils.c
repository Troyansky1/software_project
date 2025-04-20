# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "symnmf.h"

void print_matrix(double** matrix, int dim1, int dim2){
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

void print_diag_matrix(double* vector, int dim){
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
            else printf("0.0000");
            if (j < dim -1) printf("%c", ',');
            else printf("%c", '\n');
        }
    }
}

double* init_vec_mem(int dim) {
    /*
    * Allocates memory for a vector as a contiguous block.
    * Parameters:
    *   - dim: The dimension of the vector.
    * Returns:
    *   - A pointer to an array if successful.
    *   - NULL if memory allocation fails.
    */
    double *p;
    /* Allocate contiguous memory for the vector */
    p = calloc(dim, sizeof(double));  
    if (!p){
        printf("An Error Has Occurred\n");
        return NULL;  
    }
    return p;
}

double** init_matrix_mem(int dim1, int dim2) {
    /*
    * Allocates memory for an dim1 x dim2 matrix as a contiguous block.
    * Parameters:
    *   - dim1: The number of rows.
    *   - dim2: The number of columns.
    * Returns:
    *   - A pointer to an array of row pointers if successful.
    *   - NULL if memory allocation fails.
    */
    double *p;
    double **a;
    int i;
    /* Allocate contiguous memory for the matrix */
    p = calloc(dim1 * dim2, sizeof(double));  
    if (!p){
        printf("An Error Has Occurred\n");
        return NULL;  
    } 
    a = calloc(dim1, sizeof(double*));  
    if (!a) {
        free(p);
        printf("An Error Has Occurred\n");
        return NULL;
    }
    for (i = 0; i < dim1; i++)
        a[i] = p + i * dim2;

    return a;
}

void free_matrix_mem(double** matrix) {
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
    int ch;
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

int parse_line_to_array(char *line, double *row, int d) {
    /*
    * Parses a comma-separated string into an array of doubles.
    * Parameters:
    *   - line: Input string containing comma-separated values.
    *   - row: Array to store parsed double values.
    *   - d: Expected number of values (columns).
    * Returns:
    *   - 1 if parsing is successful, 0 if an error occurs.
    */
    char *ptr = line;
    int j = 0;
    while (*ptr != '\0' && *ptr != '\n' && j < d) {
        if (sscanf(ptr, "%lf", &row[j]) == 1) {
            if(*ptr == '-') ptr++;
            while ((*ptr >= '0' && *ptr <= '9')) ptr++; 
            if (*ptr == '.') {
                ptr++; 
                while ((*ptr >= '0' && *ptr <= '9')) ptr++; 
            }
            if (*ptr == ',') ptr++; 
            j++;
        } else {
            return 0; 
        }
    }
    return (j == d);
}

int init_X(FILE* file, double** X, int n, int d) {
    /*
    * Reads matrix values from a text file into X.
    * Parameters:
    *   - file: Open file pointer to read from.
    *   - X: 2D double array (size n x d) to store parsed values.
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
            free(line);
            free_matrix_mem(X);
            return 0;
        }
        free(line);
    }
    return 1;
}

double** create_X(FILE* file, int n, int d){
    /*
    * Creates and initializes a matrix X from a txt file.
    * Params:
    *   - file: Open file pointer to read from.
    * 
    * 
    * Returns:
    *   - Pointer to a n x d matrix (double**).
    *   - NULL if an error occurs.
    */
    double** X;    
    X = init_matrix_mem(n, d);
    if (!init_X(file, X, n, d)){
        return NULL;
    }
    return X;
}

