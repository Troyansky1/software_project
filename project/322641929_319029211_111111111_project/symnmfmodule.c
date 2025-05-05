# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "symnmf.h"

PyObject* sym(PyObject *self, PyObject *args);

PyObject* ddg(PyObject *self, PyObject *args);

PyObject* norm(PyObject *self, PyObject *args);

PyObject* symnmf(PyObject *self, PyObject *args);

static double** getMatrix(PyObject *Py_DF, int dim1, int dim2){
    /*
    * Recieves a matrix (dim1 x dim2) and returns it in a C format.
    * Parameters:
    *   - matrix: A PyObject matrix.
    *   - dim1: The rows dimention.
    *   - dim2: The columns dimention.
    * Returns:
    *   - The matrix in C format.
    */
    double **X;
    int i, j;
    double item;
    X = init_matrix_mem(dim1, dim2);
    for (i = 0; i < dim1; i++){
        for (j = 0; j < dim2; j++){
            item = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(Py_DF, i), j));         
            X[i][j] = item;
        }
    }
    return X;
}

PyObject *matrix_to_pyobject(double** matrix, int dim1, int dim2) {
    /*
    * Recieves a matrix (dim1 x dim2) and returns it as a PyObject.
    * Parameters:
    *   - matrix: A C matrix.
    *   - dim1: The rows dimention.
    *   - dim2: The columns dimention.
    * Returns:
    *   - The matrix as a python list of lists.
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
    double **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &Py_X)){
        return NULL;
    }
    n = PyObject_Length(Py_X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0));
    if (d < 1) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    X = getMatrix(Py_X, n, d);
    if (!X) { 
        printf("An Error Has Occurred\n");
        return NULL;
    }
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
    double **X;
    int n, d;

    if (!PyArg_ParseTuple(args, "O", &Py_X)){
        printf("An Error Has Occurred\n");
        return NULL;
    }
    n = PyObject_Length(Py_X);
    d = PyObject_Length(PyList_GetItem(Py_X, 0));
    if (d < 1) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    X = getMatrix(Py_X, n, d);
    if (!X) { 
        printf("An Error Has Occurred\n");
        return NULL;
    }
    run_ddg(X, n, d);
    free_matrix_mem(X);
    return self;
}


PyObject *norm(PyObject *self, PyObject *args){
    /*
    * Calculates and prints the normalized similarity matrix (W) using C modules.
    * Parameters:
    *   - args Py_X: The initial input matrix (X) as PyObject.
    *   - args print: A boolean, 1 if the result of symnmf is to be printed and 0 if not.
    * Returns:
    *   - The normalized similarity matrix (W) as a PyObject
    */
    PyObject *Py_X;
    double **X;
    int n, d;
    int print;
    if (!PyArg_ParseTuple(args, "Oi", &Py_X, &print)){
        printf("An Error Has Occurred\n");
        return NULL;
    }
    PyObject* first_row = PyList_GetItem(Py_X, 0);
    d = PyObject_Length(first_row);
    n = PyObject_Length(Py_X);
    if (d < 1) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    X = getMatrix(Py_X, n, d);
    if (!X) { 
        printf("An Error Has Occurred\n");
        return NULL;
    }
    double** W = init_matrix_mem(n, n);
    if (W == NULL){
        printf("An Error Has Occurred\n");
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
    *   - args print: A boolean, 1 if we want to print the result of symnmf and 0 if not.
    * Returns:
    *   - The final association matrix (H) as a PyObject
    */
    PyObject *Py_H, *Py_W, *Py_final_H;
    double **H, **W, **final_H;
    int k, n, print;

    if (!PyArg_ParseTuple(args, "OOii", &Py_H, &Py_W, &k, &print)){
        return NULL;
    }
    n = PyObject_Length(Py_H);
    H = getMatrix(Py_H, n, k);
    if (!H) { 
        printf("An Error Has Occurred\n");
        return NULL;
    } 
    W = getMatrix(Py_W, n, n);
    if (!W) { 
        printf("An Error Has Occurred\n");
        return NULL;
    }
    final_H = run_symnmf(W, H, k, n, print);
    Py_final_H = matrix_to_pyobject(final_H, n, k); 
    if (!Py_final_H) { 
        printf("An Error Has Occurred\n");
        return NULL;
    }
    free_matrix_mem(W);
    return Py_final_H;
}


static PyMethodDef symnmfMethods[] = {
    /*
    * Method table for the symnmf Python extension module.
    * Maps Python function names (as they will be called from Python)
    * to their corresponding C implementations.
    * 
    * Each entry includes:
    * - The name of the function as seen in Python.
    * - A pointer to the C function implementing it.
    * - The calling convention (e.g., METH_VARARGS indicates a tuple of arguments).
    * - A docstring for the function, used in help() and interactive sessions.
    * 
    * The list must be terminated with a sentinel {NULL, NULL, 0, NULL}.
    */
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