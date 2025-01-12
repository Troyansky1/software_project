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

    struct data_points *datapoints = init_datapoints(dpts, N, dim);
    struct centroids *head_centroid;
}

static PyObject *init_datapoints(PyObject *dpts, int N, int dim) {
    PyObject *lst;
    PyObject *item;
    struct data_points *head_point, *curr_point; 
    struct coord *head_coord, *curr_coord;
    long num;
    int j;    
    int i;
    if (!PyArg_ParseTuple(dpts, "O", &lst)) {
        return NULL;
    }

    curr_point = malloc(N * sizeof(struct data_points));
    head_point = curr_point;
    curr_coord = malloc(dim * sizeof(struct coord));
    head_coord = curr_coord;
    if (curr_point == NULL || curr_coord == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < dim; i++) {
            item = PyList_GetItem(PyList_GetItem(lst, i), j);
            curr_coord->value = PyDouble_AsDouble(item);
            curr_coord = curr_coord->next_coord;            
        }
        curr_point->coords = head_coord;
        curr_point = curr_point->next_point;
        curr_coord = malloc(dim * sizeof(struct coord));
        if (curr_coord == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }
        head_coord = curr_coord;
    }
    return head_point;
}

struct centroids* init_centroids(PyObject *cents, int K, int dim){
    PyObject *lst;
    PyObject *item;
    struct centroids *head_centroid, *curr_centroid; 
    struct coord *head_coord, *curr_coord, *curr_new_coord;
    long num;
    int j;    
    int i;
    if (!PyArg_ParseTuple(cents, "O", &lst)) {
        return NULL;
    }

    curr_centroid = malloc(K * sizeof(struct centroids));
    head_centroid = curr_centroid;

    if (curr_centroid == NULL || curr_coord == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }

    for (i = 0; i < K; i++) {
        curr_coord = malloc(dim * sizeof(struct coord));
        curr_new_coord = malloc(dim * sizeof(struct coord));
        if (curr_coord == NULL || curr_new_coord) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }
        head_coord = curr_coord;
        for (j = 0; j < dim; i++) {
            item = PyList_GetItem(PyList_GetItem(lst, i), j);
            curr_coord->value = PyDouble_AsDouble(item);
            curr_coord = curr_coord->next_coord;            
        }
        curr_centroid->coords = head_coord;
        curr_centroid->new_coords = curr_new_coord;
        curr_centroid->cnt_points = 0;
        curr_centroid = curr_centroid->next_centroid;
    }
    return head_centroid;
}