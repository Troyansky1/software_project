# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

/* Might be worth putting getmatrix and getpd in a seperate file */

static void sym(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &X)){
        return NULL;
    }
    n = PyObject_Length(X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0)); /* TODO: check if this is really D */

    X = getMatrix(Py_X, n, d);
    run_sym(X, n, d);
}

static void ddg(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &X)){
        return NULL;
    }
    n = PyObject_Length(X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0)); /* TODO: check if this is really D */
    X = getMatrix(Py_X, n, d);
    run_ddg(X, n, d);
}

static void norm(PyObject *self, PyObject *args){
    PyObject *Py_X;
    float **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &X)){
        return NULL;
    }
    n = PyObject_Length(X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0)); /* TODO: check if this is really D */
    X = getMatrix(Py_X, n, d);
    run_norm(X, n, d);
}

static PyObject* symnmf(PyObject *self, PyObject *args){
    PyObject *Py_H;
    PyObject *Py_W;
    PyObject *Py_final_H;
    float **H;
    float **W;
    float **final_H;
    int k;
    int n;

    if (!PyArg_ParseTuple(args, "OOi", &Py_H, &Py_W, &k)){
        return NULL;
    }
    H = getMatrix(Py_H, n, n); /* TODO: getmatrix (translate pandas into matrix) */
    W = getMatrix(Py_W, n, n);
    n = PyObject_Length(Py_H);
    final_H = run_symnmf(H, W, k, n);
    Py_final_H = GetPD(final_H, k); /* TODO: getPD (translate matrix into pandas) */
}

static float** getMatrix(PyObject *Py_DF, int dim1, int dim2){
    float **X;
    int i, j;
    X = init_matrix_mem(dim1, dim2);
    for (i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            X[i][j] = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(Py_DF, i), j));
        }
    }
    return X;
}
 /* make this actually do the thing we want but this is a good skeleton */
static PyObject* getPD(float **matrix, int dim1, int dim2){
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
    {"getPD",
      (PyCFunction) getPD,
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