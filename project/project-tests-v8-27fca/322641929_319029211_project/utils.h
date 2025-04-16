# ifndef UTILS_H_
# define UTILS_H_
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>

void print_matrix(float** matrix, int dim1, int dim2);

void print_diag_matrix(float* vector, int dim);

float* init_vec_mem(int dim);

float** init_matrix_mem(int dim1, int dim2);

void free_matrix_mem(float** matrix);

void get_matrix_params(int *n, int *d, FILE* file);

char* read_line(FILE* file, size_t* buffer_size);

int parse_line_to_array(char *line, float *row, int d);

int init_X(FILE* file, float** X, int n, int d);

float** create_X(FILE* file, int n, int d);

# endif
