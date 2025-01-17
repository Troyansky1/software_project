# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "cap.h"



static PyObject* mykmeanssp(PyObject *self, PyObject *args){
    PyObject *cents;
    PyObject *dpts;
    int dim;
    int iter;

    if (!PyArg_ParseTuple(args, "OOii", &cents, &dpts, &iter, &dim)){
        return NULL;
    }

    int N = PyObject_Length(dpts);
    int K = PyObject_Length(cents);

    /* struct data_points *datapoints = init_datapoints(dpts, N, dim); */
    struct centroids *centroids = init_centroids(cents, N, dim);
    printf("Centroids as received in C");
    print_centroids(centroids, K);

    return GetCentsList(centroids, dim, K);
}

static PyMethodDef kmeansMethods[] = {
    {"mykmeanssp",                   
      (PyCFunction) mykmeanssp,
      METH_VARARGS,         
      PyDoc_STR("")},
    {NULL, NULL, 0, NULL}     
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeans",
    NULL,
    -1,  
    kmeansMethods
};

PyMODINIT_FUNC PyInit_mykmeans(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
