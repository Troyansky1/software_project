# define PY_SSIZE_T_CLEAN
# include <Python.h>
# include "cap.h"

PyObject* GetCoordsList(struct coord *head_coord, int dim){
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


PyObject* GetCentsList(struct centroids *head_centroid, int dim, int K){
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
    PyObject *item, *tmp;
    struct data_points *head_point, *curr_point, *next_point; 
    struct coord *head_coord, *curr_coord, *next_coord;
    int j;    
    int i;

    curr_point = malloc(sizeof(struct data_points));
    if (curr_point == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }
    head_point = curr_point;
    curr_coord = malloc(sizeof(struct coord));
    if (curr_coord == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }
    head_coord = curr_coord;

    for (i = 0; i < N; i++) {
        for (j = 0; j < dim; j++) {
            tmp = PyList_GetItem(dpts, i);
            if (tmp == NULL){
                return NULL;
            }
            item = PyList_GetItem(tmp, j);
            if (item == NULL){
                return NULL;
            }            
            curr_coord->value = PyFloat_AsDouble(item);
            if (PyErr_Occurred()){
                return NULL;
            }     
            next_coord = malloc(sizeof(struct coord));
            if (next_coord == NULL) {
                printf("Memory allocation failed. Exiting.\n");
                return NULL;
            }   
            curr_coord->next_coord = next_coord;    
            curr_coord = curr_coord->next_coord;            
        }
        curr_point->coords = head_coord;
        next_point = malloc(sizeof(struct data_points));
        if (next_point == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }   
        curr_point->next_point = next_point; 
        curr_point = curr_point->next_point;
        curr_coord = malloc(sizeof(struct coord));
        if (curr_coord == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }
        head_coord = curr_coord;
    }
    return head_point;
}


void free_coords(struct coord* head_coord)
{
    struct coord *curr_coord, *next_coord;
    curr_coord = head_coord;
    while (curr_coord != NULL)
    {
        next_coord = curr_coord->next_coord;
        free(curr_coord);
        curr_coord = next_coord;
    }
}

void free_points(struct data_points* head_point, int N)
{
    struct data_points *curr_point, *next_point;
    int i = 0;
    curr_point = head_point;
    while (i < N)
    {
        next_point = curr_point->next_point;
        free_coords(curr_point->coords);
        free(curr_point);    
        curr_point = next_point;    
        i ++;
    }
}

void free_centroids(struct centroids* head_centroid, int K)
{
    struct centroids *curr_cent, *next_cent;
    int i = 0;
    curr_cent = head_centroid;
    while (i < K)
    {
        next_cent = curr_cent->next_centroid;
        free_coords(curr_cent->coords);
        free_coords(curr_cent->new_coords);
        free(curr_cent);   
        curr_cent = next_cent;     
        i ++;
    }
}

void free_mem(struct data_points* head_point, struct centroids* head_centroid, int N, int K)
{
    free_points(head_point, N);
    free_centroids(head_centroid, K);
}


struct centroids *init_centroids(PyObject *cents, int K, int dim){
    /* Py obj to C struct. */
    PyObject *item, *tmp;
    struct centroids *head_centroid, *curr_centroid, *next_centroid; 
    struct coord *head_coord, *curr_coord, *curr_new_coord, *next_coord;
    int j;    
    int i;

    curr_centroid = malloc(sizeof(struct centroids));
    head_centroid = curr_centroid;

    if (curr_centroid == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }

    for (i = 0; i < K; i++) {
        curr_coord = malloc(sizeof(struct coord));
        curr_new_coord = malloc(sizeof(struct coord));
        if (curr_coord == NULL || curr_new_coord == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }
        head_coord = curr_coord;
        for (j = 0; j < dim; j++) {
            tmp = PyList_GetItem(cents, i);
            if (tmp == NULL){
                return NULL;
            }
            item = PyList_GetItem(tmp, j);
            if (item == NULL){
                return NULL;
            }
            curr_coord->value = PyFloat_AsDouble(item);
            if (PyErr_Occurred()){
                return NULL;
            }
            next_coord = malloc(sizeof(struct coord));
            if (next_coord == NULL) {
                printf("Memory allocation failed. Exiting.\n");
                return NULL;
            }   
            curr_coord->next_coord = next_coord; 
            curr_coord = curr_coord->next_coord;            
        } 
        curr_centroid->coords = head_coord;
        curr_centroid->new_coords = curr_new_coord;
        curr_centroid->cnt_points = 0;

        next_centroid = malloc(sizeof(struct centroids));
        if (next_centroid == NULL) {
            printf("Memory allocation failed. Exiting.\n");
            return NULL;
        }   
        curr_centroid->next_centroid = next_centroid; 
        curr_centroid = curr_centroid->next_centroid;
    }
    return head_centroid;
}
