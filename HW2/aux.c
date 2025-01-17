# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "cap.h"

static PyObject* GetCoordsList(struct coord *head_coord, int dim){
    /* C struct to  py list */
    double val;
    PyObject* python_coords_list;
    PyObject* python_val;

    python_coords_list = PyList_New(dim);
    for (int i = 0; i < dim; ++i)
    {
        val = head_coord->value;
        python_val = Py_BuildValue("d", val);
        PyList_SetItem(python_coords_list, i, python_val);
        head_coord = head_coord->next_coord;
    }
    return python_coords_list;
}


static PyObject* GetCentsList(struct centroids *head_centroid, int dim, int K){
    /* C struct to py list */
    PyObject* coords;
    PyObject* python_centroids_list;
    PyObject* python_val;

    python_centroids_list = PyList_New(K);
    for (int i = 0; i < K; ++i)
    {
        coords = GetCoordsList(head_centroid->coords, dim);
        python_val = Py_BuildValue("O", coords);
        PyList_SetItem(python_centroids_list, i, python_val);
        head_centroid = head_centroid->next_centroid;
    }
    return python_centroids_list;
}

struct data_points *init_datapoints(PyObject *dpts, int N, int dim) {
    /* Py obj to C struct. */
    PyObject *lst;
    PyObject *item;
    struct data_points *head_point, *curr_point; 
    struct coord *head_coord, *curr_coord;
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
            curr_coord->value = PyLong_AsDouble(item);
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

struct centroids *init_centroids(PyObject *cents, int K, int dim){
    /* Py obj to C struct. */
    PyObject *lst;
    PyObject *item;
    struct centroids *head_centroid, *curr_centroid; 
    struct coord *head_coord, *curr_coord, *curr_new_coord;
    int j;    
    int i;
    if (!PyArg_ParseTuple(cents, "O", &lst)) {
        return NULL;
    }

    curr_centroid = malloc(K * sizeof(struct centroids));
    head_centroid = curr_centroid;

    if (curr_centroid == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }

    for (i = 0; i < K; i++) {
        curr_coord = malloc(dim * sizeof(struct coord));
        curr_new_coord = malloc(dim * sizeof(struct coord));
        if (curr_coord == NULL || curr_new_coord == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }
        head_coord = curr_coord;
        for (j = 0; j < dim; i++) {
            item = PyList_GetItem(PyList_GetItem(lst, i), j);
            curr_coord->value = PyLong_AsDouble(item);
            curr_coord = curr_coord->next_coord;            
        }
        curr_centroid->coords = head_coord;
        curr_centroid->new_coords = curr_new_coord;
        curr_centroid->cnt_points = 0;
        curr_centroid = curr_centroid->next_centroid;
    }
    return head_centroid;
}
