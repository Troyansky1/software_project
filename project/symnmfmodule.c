# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"


static float** getMatrix(PyObject *Py_DF, int dim1, int dim2){
    float **X;
    int i, j;
    float item;
    X = init_matrix_mem(dim1, dim2);
    for (i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            item = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(Py_DF, i), j));         
            X[i][j] = item;
        }
    }
    return X;
}

 static PyObject* getVector(float *vec, int dim){
    PyObject *Py_vec;
    int i;
    Py_vec = PyList_New(dim);
    for (i = 0; i < dim; i++){
        PyList_SetItem(Py_vec, i, PyFloat_FromDouble(vec[i]));        
    }
    return Py_vec;
}


PyObject *sym(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &Py_X)){
        return NULL;
    }
    /* warning: passing argument 1 of ‘PyObject_Size’ from incompatible pointer type*/
    n = PyObject_Length(Py_X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0)); /* TODO: check if this is really D */

    X = getMatrix(Py_X, n, d);
    run_sym(X, n, d);
    return self;
}

PyObject *ddg(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &Py_X)){
        return NULL;
    }
    n = PyObject_Length(Py_X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0)); /* TODO: check if this is really D */
    X = getMatrix(Py_X, n, d);
    run_ddg(X, n, d);
    return self;
}


PyObject *matrix_to_pyobject(float** matrix, int dim1, int dim2) {
    PyObject* python_list = PyList_New(dim1); 
    if (!python_list) return NULL;  

    for (int i = 0; i < dim1; ++i) {
        PyObject* row_list = PyList_New(dim2);  
        if (!row_list) {
            /*  Cleanup outer list if inner list fails*/
            Py_DECREF(python_list); 
            return NULL;
        }
        for (int j = 0; j < dim2; j++) {
            PyObject* python_val = PyFloat_FromDouble(matrix[i][j]);  
            if (!python_val) {
                Py_DECREF(row_list);
                Py_DECREF(python_list);
                return NULL;
            }
            PyList_SetItem(row_list, j, python_val);  
        }
        PyList_SetItem(python_list, i, row_list);  
    }
    return python_list;  
}



PyObject *norm(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;
    if (!PyArg_ParseTuple(args, "O", &Py_X)){
        printf("Error: PyArg_ParseTuple\n");
        return NULL;
    }
    if (!PyList_Check(Py_X)) {
        printf("Error: Py_X is not a list\n");
        return NULL;
    }
    n = PyObject_Length(Py_X);
    if (n < 1) {
        printf("Error: Py_X is empty\n");
        return NULL;
    }
    PyObject* first_row = PyList_GetItem(Py_X, 0);
    if (!first_row) {
        printf("Error: Failed to get first item from Py_X\n");
        return NULL;
    }
    if (!PyList_Check(first_row)) {
        printf("Error: First item in Py_X is not a list\n");
        return NULL;
    }
    d = PyObject_Length(first_row);
    if (d < 1) {
        printf("Error: First row is empty\n");
        return NULL;
    }
    X = getMatrix(Py_X, n, d);
    if (!X) { 
        printf("Error: getMatrix() failed\n");
        return NULL;
    }
    float** W = init_matrix_mem(n, n);
    if (W == NULL){
        printf("Error: init_matrix_mem failed\n");
        return NULL;
    } 
    run_norm(W, X, n, d); 
    return matrix_to_pyobject(W, n, n);
}

 /* make this actually do the thing we want but this is a good skeleton */
 static PyObject* getDF(float **matrix, int dim1, int dim2){
    PyObject *Py_DF;
    int i, j;
    Py_DF = PyList_New(dim1);
    for (i = 0; i < dim1; i++){
        PyObject *Py_row = PyList_New(dim2);
        for (j = 0; j < dim2; j++){
            PyList_SetItem(Py_row, j, PyFloat_FromDouble(matrix[i][j]));
        }
        PyList_SetItem(Py_DF, i, Py_row);
    }
    return Py_DF;
}


PyObject* symnmf(PyObject *self, PyObject *args){
    PyObject *Py_H;
    PyObject *Py_W;
    PyObject *Py_final_H;
    float **H;
    float **W;
    float **final_H;
    int k;
    int n;
    int print;

    if (!PyArg_ParseTuple(args, "OOii", &Py_H, &Py_W, &k, &print)){
        return NULL;
    }
    n = PyObject_Length(Py_H);
    H = getMatrix(Py_H, n, k); /* TODO: getmatrix (translate pandas into matrix) */
    W = getMatrix(Py_W, n, n);
    final_H = run_symnmf(H, W, k, n, print);
    Py_final_H = getDF(final_H, n, k); /* TODO: getDF (translate matrix into pandas) */
    return Py_final_H;
}

PyObject* clustering_sol(PyObject *self,PyObject *args){
    PyObject *Py_H;
    PyObject *Py_W;
    PyObject *Py_sol;
    float **H;
    float **W;
    float **final_H;
    float *sol;
    int k;
    int n;

    if (!PyArg_ParseTuple(args, "OOi", &Py_H, &Py_W, &k)){
        return NULL;
    }
    n = PyObject_Length(Py_H);
    H = getMatrix(Py_H, n, k); /* TODO: getmatrix (translate pandas into matrix) */
    W = getMatrix(Py_W, n, n);
    final_H = run_symnmf(H, W, k, n, 1);
    sol = derive_clustering_sol(final_H, n, k);
    Py_sol = getVector(sol, n);
    return Py_sol;
}


static PyMethodDef symnmfMethods[] = {
    {"sym",                   
      (PyCFunction) sym,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"ddg",
      (PyCFunction) ddg,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"norm",
      (PyCFunction) norm,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"symnmf",
      (PyCFunction) symnmf,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"getMatrix",
      (PyCFunction) getMatrix,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"getDF",
      (PyCFunction) getDF,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"clustering_sol",
    (PyCFunction) clustering_sol,
    METH_VARARGS,         
    PyDoc_STR("")},
    {NULL, NULL, 0, NULL}     
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,  
    symnmfMethods
};

PyMODINIT_FUNC PyInit_symnmf(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}