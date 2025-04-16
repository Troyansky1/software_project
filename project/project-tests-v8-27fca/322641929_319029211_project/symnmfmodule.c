# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

PyObject* sym(PyObject *self, PyObject *args);

PyObject* ddg(PyObject *self, PyObject *args);

PyObject* norm(PyObject *self, PyObject *args);

PyObject* symnmf(PyObject *self, PyObject *args);

static float** getMatrix(PyObject *Py_DF, int dim1, int dim2){
    /*
    * Recieves a matrix (dim1 x dim2) and returns it in a C format.
    * Parameters:
    *   - matrix: A PyObject matrix.
    *   - dim1: The rows dimention.
    *   - dim2: The columns dimention.
    * Returns:
    *   - The matrix in C format.
    */
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

PyObject *matrix_to_pyobject(float** matrix, int dim1, int dim2) {
    /*
    * Recieves a matrix (dim1 x dim2) and returns it as a PyObject.
    * Parameters:
    *   - matrix: A C matrix.
    *   - dim1: The rows dimention.
    *   - dim2: The columns dimention.
    * Returns:
    *   - A python list of lists.
    */
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
    free_matrix_mem(matrix);

    return python_list;  
}

PyObject *sym(PyObject *self, PyObject *args){
    /*
    * Calculates and prints the similarity matrix (A) using C modules.
    * Parameters:
    *   - args Py_X: The initial input matrix (X) as PyObject.
    */
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
    free_matrix_mem(X);
    return self;
}

PyObject *ddg(PyObject *self, PyObject *args){
    /*
    * Calculates and prints The diagonal degree matrix (D) using C modules.
    * Parameters:
    *   - args Py_X: The initial input matrix (X) as PyObject.
    */
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
    free_matrix_mem(X);
    return self;
}


PyObject *norm(PyObject *self, PyObject *args){
    /*
    * Calculates and prints The normalized similarity matrix (W) using C modules.
    * Parameters:
    *   - args Py_X: The initial input matrix (X) as PyObject.
    *   - args print: A bollean, 1 if we want to print the result of symnmf and 0 if not.
    * Returns:
    *   - The finale association matrix (H) as a PyObject
    */
    PyObject *Py_X;
    float **X;
    int n, d;
    int print;
    if (!PyArg_ParseTuple(args, "Oi", &Py_X, &print)){
        printf("Error: PyArg_ParseTuple\n");
        return NULL;
    }
    PyObject* first_row = PyList_GetItem(Py_X, 0);
    d = PyObject_Length(first_row);
    n = PyObject_Length(Py_X);
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
    run_norm(W, X, n, d, print); 
    free_matrix_mem(X);
    return matrix_to_pyobject(W, n, n);
}


PyObject* symnmf(PyObject *self, PyObject *args){
    /*
    * Calculates a symmetric Non-negative Matrix Factorization (symnmf) by calling c modules.
    * Parameters:
    *   - args Py_H: The initial association matrix (H) as PyObject.
    *   - args Py_W: The normalized similarity matrix as PyObject.
    *   - args k: The number of clusters.
    *   - args print: A bollean, 1 if we want to print the result of symnmf and 0 if not.
    */
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
    H = getMatrix(Py_H, n, k); 
    W = getMatrix(Py_W, n, n);
    final_H = run_symnmf(W, H, k, n, print);
    Py_final_H = matrix_to_pyobject(final_H, n, k); 
    free_matrix_mem(W);
    return Py_final_H;
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
    {NULL, NULL, 0, NULL}     
};

static struct PyModuleDef moduledef = {
    /*
    * Module definition for symnmf Python C extension.
    * Parameters:
    *   - m_base: Internal use.
    *   - m_name: Name of the module ("symnmf").
    *   - m_doc: Module documentation (NULL).
    *   - m_size: Size of module state (-1 means global state).
    *   - m_methods: Array of methods provided by this module.
    */
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,  
    symnmfMethods
};

PyMODINIT_FUNC PyInit_symnmf(void){
    /*
    * Initialize the symnmf module.
    * Returns:
    *   - A Python module object, or NULL on failure.
    */
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}