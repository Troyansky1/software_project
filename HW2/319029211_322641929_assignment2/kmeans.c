# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "cap.h"

#ifdef DEBUG
#define DPRINT printf("Debug print at %d\n", __LINE__)
#else
#define DPRINT {}
#endif

int num_points;

void print_point(struct data_points *point, int dim);

void print_centroids(struct centroids *head_centroid, int K, int dim);

double euclid_dist(struct coord* data_point1, struct coord* data_point2, int dim);

void assign_to_cluster(struct data_points* point, struct centroids* head_centroid, int K, int dim);

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points, int dim);

int update_centroids_and_check_covergence(struct centroids* cents, double eps, int K, int dim);


void print_point(struct data_points *point, int dim){
    /* Debug printint of a data point. */
    struct coord *coord;
    int i;
    coord = point->coords;
    printf("Printing point in index %d\n", point->idx);
    printf("Printing point values:\n");  
    for (i = 0; i < dim; i++){
        printf("%f ",coord->value);
        coord = coord->next_coord;
    }
    printf("\n");
}

void print_centroids(struct centroids *head_centroid, int K, int dim){
    struct coord* curr;
    int i = 0;
    int j;
      while (i < K)
    {
        curr = head_centroid->coords;
        for (j=0; j < dim -1; j++)
        {
            printf("%.4f,", curr->value);
            curr = curr->next_coord;
        }
        printf("%.4f\n", curr->value);
        head_centroid = head_centroid->next_centroid;
        i ++;
    }
}


double euclid_dist(struct coord* data_point1, struct coord* data_point2, int dim){
    /* Calculate the euclidean distance between 2 points.
    Assuming they have the same number of coordinates. */
    double sum = 0;
    int i;
    double curr_diff;
    for (i = 0; i < dim; i++) {     
        curr_diff = (data_point1->value) - (data_point2->value);
        sum += pow(curr_diff, 2);
        data_point1 = data_point1->next_coord;
        data_point2 = data_point2->next_coord;
    }
    return sqrt(sum);
}

void assign_to_cluster(struct data_points* point, struct centroids* head_centroid, int K, int dim){
    /* Assign a point to the closest cluster */
    struct centroids *curr_centroid, *min_cent;
    struct coord *cent_new_coord, *curr_pt_coord;
    double min_dist = __INT_MAX__;
    int i;
    int j;
    double new_coord;

    double dist;
    curr_centroid = head_centroid;
    min_cent = curr_centroid;
    /* Find the closest centroid to the point. */
    for (i = 0; i < K; i++){
        dist = euclid_dist(point->coords, curr_centroid->coords, dim);
        
        if (dist <= min_dist){
            min_dist = dist;
            min_cent = curr_centroid;
        }        
        curr_centroid = curr_centroid->next_centroid;        
    }
    /* update field in point. */    
    point->centroid = min_cent;    
    /* Update fields in centroid (min_cent).
        add 1 to the cnt_points
        add the values of the point to the new coords field in the centroid */
    min_cent->cnt_points += 1.0;
    curr_pt_coord = point->coords;
    cent_new_coord = min_cent->new_coords;    
    for (j = 0; j<dim; j++){
        new_coord = curr_pt_coord->value;
        cent_new_coord->value += new_coord;            
        curr_pt_coord = curr_pt_coord->next_coord;
        cent_new_coord = cent_new_coord->next_coord; 
    } 
}

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points, int dim){
    /* Assign each point to a cluster (centroid) */
    struct data_points *curr_point;    
    int i;
    curr_point = head_point;
    for (i = 0; i < num_points; i++){
        assign_to_cluster(curr_point, head_centroid, K, dim);    
        curr_point = curr_point->next_point;        
    }    
}

int update_centroids_and_check_covergence(struct centroids* cents, double eps, int K, int dim)
{
    struct coord* point_coords;
    struct coord* cent_coords;
    struct coord* prev_coords = malloc(sizeof(struct coord));
    struct coord* prev_head;
    /* Of free coords
    struct coord *curr_coord, *next_coord; */
    int num_pts;
    double dist;
    int ret = 1;
    int i;
    int j;

    cent_coords = cents->coords;
    prev_head = prev_coords;

    for (i = 0; i < dim; i++){
        prev_coords->next_coord = malloc(sizeof(struct coord));
        prev_coords = prev_coords->next_coord;
    }

    prev_coords->next_coord = NULL;
    prev_coords = prev_head;

    for (j = 0; j < K; j++)
    {
        num_pts = cents->cnt_points;
        cents->cnt_points = 0.0;
        cent_coords = cents->coords;
        /* Save the coordinates to check the convvergance later. */
        for (i = 0; i < dim; i++)
        {
            prev_coords->value = cent_coords->value;
            prev_coords = prev_coords->next_coord;
            cent_coords = cent_coords->next_coord;
        }

        cent_coords = cents->coords;
        point_coords = cents->new_coords;
        prev_coords = prev_head;
        /* Iterate over each coordinate and update its value as the mean of the points in its cluster.*/
        for (i = 0; i < dim; i++)
        {   
            /* If num_pts is 0, value must be 0 as well and does not need to be updated!*/
            if (num_pts > 0){
                cent_coords->value = (point_coords->value)/num_pts;
            }        
            point_coords->value = 0.0;
            cent_coords = cent_coords->next_coord;
            point_coords = point_coords->next_coord;
        }
        cent_coords = cents->coords;

        dist = euclid_dist(cent_coords, prev_coords, dim);
        if (dist >= eps)
        {
            ret = 0;
        }
        cents = cents->next_centroid;        
    }
    prev_head = prev_coords;
    
    return ret;
}

struct centroids* run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points, int dim, double eps){
    int conv_flag = 0;
    int i = 0;    
    while ((i < iter) && (conv_flag == 0))
    {
        assign_to_clusters(head_point, head_centroid, K, num_points, dim);
        conv_flag = update_centroids_and_check_covergence(head_centroid, eps, K, dim);   
        i ++;
    }
    print_centroids(head_centroid, K, dim);
    return head_centroid;
}

int main(int argc, char **argv){    
    return 0;
    
}