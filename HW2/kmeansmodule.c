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

struct data_points* init_datapoints(PyObject *dpts, int N)
{
    /* Initialization- code taken from lecture notes. */
    struct coord *head_coord, *curr_coord;
    struct data_points *head_point, *curr_point ;
    
    double n;
    int cnt = 0;
    int i;
    int j;

    head_coord = malloc(sizeof(struct coord));
    if (head_coord == NULL) {
        printf("An Error Has Occurred\n");
        return NULL; 
    }
    curr_coord = head_coord;
    curr_coord->next_coord = NULL;

    head_point = malloc(sizeof(struct data_points));
    if (head_point == NULL) {
        printf("An Error Has Occurred\n");
        free(head_coord); 
        return NULL; 
    }
    head_point->idx = -1;
    curr_point = head_point;
    curr_point->next_point = NULL;
    
    for (i = 0; i < N; i++)
    {  
        for (j = 0; j < dim - 1; j++)
        {
            curr_coord->value = n;
            curr_coord->next_coord = malloc(sizeof(struct coord));
            if (curr_coord->next_coord == NULL) {
                printf("An Error Has Occurred\n");
                free_points(head_point, cnt); 
                return NULL; 
            }
        curr_coord = curr_coord->next_coord;
        curr_coord->next_coord = NULL;
        }
        curr_coord->value = ;
        curr_point->coords = head_coord;
        curr_point->idx = cnt;
        curr_point->next_point = malloc(sizeof(struct data_points));     
        if (curr_point->next_point == NULL) {
            printf("An Error Has Occurred\n");
            free_points(head_point, cnt); 
            return NULL; 
        }
        curr_point = curr_point->next_point;   
        curr_point->idx = -1;         
        curr_point->next_point = NULL;
        head_coord = malloc(sizeof(struct coord));
        if (head_coord == NULL) {
            printf("An Error Has Occurred\n");
            free_points(head_point, cnt); 
            return NULL; 
        }
        curr_coord = head_coord;
        curr_coord->next_coord = NULL;
        cnt ++;
        continue;
        /* last value in the coordinate- Files end with an empty line. */
    }
    free_coords(head_coord);
    free(curr_point);
    num_points = cnt;
    if (cnt == 0)  {
        return NULL;
    }
    return head_point;
} 

struct centroids* init_centroids(int K, struct data_points* data_point){
    /* Initialize centroids with values of the first K data points. */
    struct data_points *curr_point;
    struct centroids *head_centroid, *curr_centroid;
    struct coord *head_coord, *curr_cent_coord, *curr_pt_coord, *head_new_coords, *curr_new_coord;
    
    int i;
    curr_point = data_point;
    head_centroid = malloc(sizeof(struct centroids));
    if (head_centroid == NULL) {
        printf("An Error Has Occurred\n");
        free_centroids(head_centroid, 1);         
        return NULL;
    }
    curr_centroid = head_centroid;
    for (i = 0; i < K; i++){
        /* At initiation, there are no points allocated to any centroid, and no need to update the coordinations.*/
        curr_centroid->cnt_points = 0.0;

        /* Init new coords */
        head_new_coords = malloc(sizeof(struct coord));
        if (head_new_coords == NULL) {
            printf("An Error Has Occurred\n");
            free_centroids(head_centroid, i); 
            return NULL;
        }
        curr_new_coord = head_new_coords;
        curr_centroid->new_coords = head_new_coords;

        /*Copy value of coords from the first K points to the K centroids. */
        head_coord = malloc(sizeof(struct coord));
        if (head_coord == NULL) {
            printf("An Error Has Occurred\n");            
            free_centroids(head_centroid, i); 
            return NULL;
        }
        curr_cent_coord = head_coord;  
        curr_pt_coord = curr_point->coords;
        curr_centroid->coords = head_coord;    
        /* Copy each value in the coordinate*/  
        while(curr_pt_coord != NULL){                        
            curr_cent_coord->value = curr_pt_coord->value;     
            curr_pt_coord = curr_pt_coord->next_coord;
            curr_new_coord->value = 0.0;
            if (curr_pt_coord == NULL){
                curr_cent_coord->next_coord = NULL;
                curr_new_coord->next_coord = NULL;
            }
            else{
                curr_cent_coord->next_coord = malloc(sizeof(struct coord));
                if (curr_cent_coord->next_coord == NULL) {
                    printf("An Error Has Occurred\n");
                    free_centroids(head_centroid, i); 
                    return NULL; 
                }
                curr_cent_coord = curr_cent_coord->next_coord;

                curr_new_coord->next_coord = malloc(sizeof(struct coord));
                if (curr_new_coord->next_coord == NULL) {
                    printf("An Error Has Occurred\n");
                    free_centroids(head_centroid, i); 
                    return NULL; 
                }
                curr_new_coord = curr_new_coord->next_coord;
            }            
        }        
        if (i != K-1){
            curr_point = curr_point->next_point;
            curr_centroid->next_centroid = malloc(sizeof(struct centroids));
            if (head_centroid == NULL) {
                printf("An Error Has Occurred\n");
                free_centroids(head_centroid, i);         
                return NULL;
            }
            curr_centroid = curr_centroid->next_centroid;
        }   
    }    
    curr_centroid->next_centroid = head_centroid;
    return head_centroid;
}