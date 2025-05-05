# ifndef UTILS_H_
# define UTILS_H_
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>

void print_matrix(double** matrix, int dim1, int dim2);

void print_diag_matrix(double* vector, int dim);

double* init_vec_mem(int dim);

double** init_matrix_mem(int dim1, int dim2);

void free_matrix_mem(double** matrix);

void get_matrix_params(int *n, int *d, FILE* file);

char* read_line(FILE* file, size_t* buffer_size);

int parse_line_to_array(char *line, double *row, int d);

int init_X(FILE* file, double** X, int n, int d);

double** create_X(FILE* file, int n, int d);

# endif
