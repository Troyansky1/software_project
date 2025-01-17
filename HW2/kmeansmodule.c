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

PyObject* fit(PyObject *cents, PyObject *dpts,int iter, int N, int K, int dim){
    /* Init data points and centroids in python.
        calls kmeans.c, which returns the centroids in struct.
        returns centroids in python.
    */
    struct data_points *head_point;
    struct centroids *head_centroid; 
    head_point = init_datapoints(dpts, N, dim);
    head_centroid = init_centroids(cents, K, dim);
    head_centroid = run_kmeans(head_point, head_centroid, K, iter, N);
    cents = GetCentsList(head_centroid, dim, K);
    return cents;
}

static PyMethodDef kmeansMethods[] = {
    {"mykmeanssp",                   
      (PyCFunction) mykmeanssp,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"GetCoordsList",
      (PyCFunction) GetCoordsList,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"GetCentsList",
      (PyCFunction) GetCentsList,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"init_datapoints",
      (PyCFunction) init_datapoints,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"init_centroids",
      (PyCFunction) init_centroids,
      METH_VARARGS,         
      PyDoc_STR("")},
    {"fit",
      (PyCFunction) fit,
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

