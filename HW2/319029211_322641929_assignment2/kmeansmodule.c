# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "cap.h"


static PyObject* mykmeanssp(PyObject *self, PyObject *args){
    PyObject *cents;
    PyObject *dpts;
    int dim;
    int iter;
    int N;

    if (!PyArg_ParseTuple(args, "OOii", &cents, &dpts, &iter, &dim)){
        return NULL;
    }

    N = PyObject_Length(dpts);
    /* int K = PyObject_Length(cents);*/

    /* struct data_points *datapoints = init_datapoints(dpts, N, dim); */
    struct centroids *centroids = init_centroids(cents, N, dim);
    if (centroids == NULL){
      /* printf("Failure in init centroids in C"); */
      return NULL;
    }
    /* printf("Centroids as received in C"); */
    /* print_centroids(centroids, K); */
    return self;
    /* return GetCentsList(centroids, dim, K); */
}


static PyObject* fit(PyObject *self, PyObject *args){
    /* Init data points and centroids in python.
        calls kmeans.c, which returns the centroids in struct.
        returns centroids in python.
    
    struct data_points *head_point;
    head_point = init_datapoints(dpts, N, dim);
    */
    PyObject *cents;
    PyObject *dpts;
    struct data_points *head_point;
    struct centroids *head_centroid;
    int dim;
    int iter;
    int N;
    int K;
    double eps;

    if (!PyArg_ParseTuple(args, "OOiiiid", &cents, &dpts, &iter, &N, &K, &dim, &eps)){
        return NULL;
    }
    head_point = init_datapoints(dpts, N, dim);
    head_centroid = init_centroids(cents, K, dim);
    if (head_point == NULL || head_centroid == NULL){
      printf("An Error Has Occurred\n");
      /* exit(1);*/
    }
    head_centroid = run_kmeans(head_point, head_centroid, K, iter, N, dim, eps); 
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
    "mykmeanssp",
    NULL,
    -1,  
    kmeansMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}

