# ifndef SYMNMF_H_
# define SYMNMF_H_
# include <Python.h>

/* Functions in symnmfmodule.c */

void sym(PyObject *self, PyObject *args);

void ddg(PyObject *self, PyObject *args);

void norm(PyObject *self, PyObject *args);

PyObject* symnmf(PyObject *self, PyObject *args);

float** init_matrix_mem(int dim1, int dim2);

float** run_symnmf(float** W, float** H, int k, int n);

void run_sym(float **X, int n, int d);

void run_ddg(float **X, int n, int d);

void run_norm(float **X, int n, int d);


# endif