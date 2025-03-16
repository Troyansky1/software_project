# ifndef SYMNMF_H_
# define SYMNMF_H_
# include <Python.h>

/* Functions in symnmfmodule.c */

PyObject* sym(PyObject *self, PyObject *args);

PyObject* ddg(PyObject *self, PyObject *args);

PyObject* norm(PyObject *self, PyObject *args);

PyObject* symnmf(PyObject *self, PyObject *args);

void print_matrix(float** matrix, int dim1, int dim2);

float** init_matrix_mem(int dim1, int dim2);

float** run_symnmf(float** W, float** H, int k, int n, int print);

void run_sym(float **X, int n, int d);

void run_ddg(float **X, int n, int d);

float** run_norm(float **W, float **X, int n, int d);

float* derive_clustering_sol(float** H, int n, int k);

# endif