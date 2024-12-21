# include <stdio.h>
# include <stdlib.h>
# include <math.h>
#include <assert.h>

#ifdef DEBUG
#define DPRINT printf("Debug print at %d\n", __LINE__)
#else
#define DPRINT {}
#endif

/* Structs should be documented better!*/

/* A value in a data point. A-cyclyc linked list of coords. */
struct coord
{
    double value;
    struct coord *next_coord;
};

/* A data point with values, cyclyc linked list of points. */
struct data_points
{
    struct data_points *next_point;
    struct coord *coords;
    int idx;
    struct centroids *centroid; /*We onlly need the one we point to, not the whole list. */
};

/* A centroid with values, cyclyc linked list of centroids. */
struct centroids
{
    struct centroids *next_centroid;
    struct coord *coords;
    int cnt_points; /* So we can calc the new mean */
    struct coord *new_coords; /* So we can add up coord and calc new mean */
};

int num_points;

int coord_len;

void print_point(struct data_points *point);

void print_centroids(struct centroids *head_centroid, int K);

int get_k(char **argv, int N);

int get_iter(char **argv);

struct data_points* init_datapoints();

struct centroids* init_centroids(int K, struct data_points* data_point);

double euclid_dist(struct coord* data_point1, struct coord* data_point2);

void assign_to_cluster(struct data_points* point, struct centroids* head_centroid, int K);

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points);

int update_centroids_and_check_covergence(struct centroids* cents, double eps, int K);

void run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points);

void free_coords(struct coord* head_coord);

void free_points(struct data_points* head_point, int N);

void free_centroids(struct centroids* head_centroid, int K);

void free_mem(struct data_points* head_point, struct centroids* head_centroid, int N, int K);

void count(struct coord* coord);

void print_point(struct data_points *point){
    /* Debug printint of a data point. */
    struct coord *coord;
    coord = point->coords;
    printf("Printing point in index %d\n", point->idx);
    printf("Printing point values:\n");  
    while (coord != NULL){
        printf("%f ",coord->value);
        coord = coord->next_coord;
    }
    printf("\n");
}

void print_centroids(struct centroids *head_centroid, int K){
    struct coord* curr;
    int i = 0;

      while (i < K)
    {
        curr = head_centroid->coords;
        while (curr->next_coord != NULL)
        {
            printf("%.4f, ", curr->value);
            curr = curr->next_coord;
        }
        printf("%.4f\n", curr->value);
        head_centroid = head_centroid->next_centroid;
        i ++;
    }
}

int get_num_points(struct data_points* head_point){
    struct data_points* curr_point;
    int num_points = 0;

    curr_point = head_point;    
    while (curr_point->idx != -1){
        num_points +=1;
        curr_point = curr_point->next_point;
    }
    return num_points;
}

int get_k(char **argv, int N){
    /* Validates the value of N as received in argv.
    valid values:
    1 < K < N, K is a natural number.
    */
    long K;
    /* TODO: Handle exceptions */
    K = strtol(argv[1], NULL, 10);
    /* Check values */
    if (K <= 1 || K >= N){
        printf("Invalid number of clusters!\n");
        return -1;
    }
    return K;
}


int get_iter(char **argv){
    /* Validates the value of iter as received in argv.
    valid values:
    1 < iter < 1000, iter is a natural number.
    */
    long iter;

    /* TODO: Handle exceptions */
    iter = strtol(argv[2], NULL, 10); 
    /* Check values */
    if (iter <= 1 || iter >= 1000){
        printf("Invalid maximum iteration!");
        return -1;
    }
    printf("iter = %ld\n", iter);    
    
    return iter;
}


struct data_points* init_datapoints()
{
    /* Initialization- code taken from lecture notes. */
    struct coord *head_coord, *curr_coord;
    struct data_points *head_point, *curr_point ;
    
    int rows = 0;
    double n;
    char c;
    int cnt = 0;

    head_coord = malloc(sizeof(struct coord));
    assert(head_coord != NULL);
    curr_coord = head_coord;
    curr_coord->next_coord = NULL;

    head_point = malloc(sizeof(struct data_points));
    assert(head_point!= NULL);
    curr_point = head_point;
    curr_point->next_point = NULL;

    while (scanf("%lf%c", &n, &c) == 2)
    {        
        /* last value in the coordinate- Files end with an empty line. */
        if (c == '\n')
        {        
            curr_coord->value = n;
            curr_point->coords = head_coord;
            curr_point->idx = rows;
            curr_point->next_point = malloc(sizeof(struct data_points));     
            assert(curr_point->next_point != NULL);
            curr_point = curr_point->next_point;   
            curr_point->idx = -1;         
            curr_point->next_point = NULL;
            head_coord = malloc(sizeof(struct coord));
            assert(head_coord != NULL);
            curr_coord = head_coord;
            curr_coord->next_coord = NULL;
            rows ++;
            cnt ++;
            continue;
        }
        /* Add another value to same coordinate */
        curr_coord->value = n;
        curr_coord->next_coord = malloc(sizeof(struct coord));
        assert(curr_coord->next_coord != NULL);
        curr_coord = curr_coord->next_coord;
        curr_coord->next_coord = NULL;        
    }
    curr_point = NULL;  
    num_points = cnt;  
    /* Returning the last point, so we can infer the anount of points.
        A better solution should be found TODO */   
    /* ^ solved maybe? */
    return head_point;
}

struct centroids* init_centroids(int K, struct data_points* data_point){
    /* Initialize centroids with values of the first K data points. */
    struct data_points *curr_point;
    struct centroids *head_centroid, *curr_centroid;
    struct coord *head_coord, *curr_cent_coord, *curr_pt_coord, *head_new_coords, *curr_new_coords;
    
    int i;
    curr_point = data_point;
    head_centroid = malloc(sizeof(struct centroids));
    assert(head_centroid != NULL);
    curr_centroid = head_centroid;
    for (i = 0; i < K; i++){
        /* At initiation, there are no points allocated to any centroid, and no need to update the coordinations.*/
        curr_centroid->cnt_points = 0;

        /* Init new coords */
        head_new_coords = malloc(sizeof(struct coord));
        assert(head_new_coords != NULL);
        curr_new_coords = head_new_coords;
        curr_centroid->new_coords = head_new_coords;

        /*Copy value of coords from the first K points to the K centroids. */
        head_coord = malloc(sizeof(struct coord));
        assert(head_coord != NULL);
        curr_cent_coord = head_coord;  
        curr_pt_coord = curr_point->coords;
        curr_centroid->coords = head_coord;    
        /* Copy each value in the coordinate*/  
        while(curr_pt_coord != NULL){                        
            curr_cent_coord->value = curr_pt_coord->value;     
            curr_pt_coord = curr_pt_coord->next_coord;
            curr_new_coords->value = 0;
            /*TODO: I'm not sure this is the most elegant way to do this */
            if (curr_pt_coord == NULL){
                curr_cent_coord->next_coord = NULL;
                curr_new_coords->next_coord = NULL;
            }
            else{
                curr_cent_coord->next_coord = malloc(sizeof(struct coord));
                assert(curr_cent_coord->next_coord != NULL);
                curr_cent_coord = curr_cent_coord->next_coord;

                curr_new_coords->next_coord = malloc(sizeof(struct coord));
                assert(curr_new_coords->next_coord != NULL);
                curr_new_coords = curr_new_coords->next_coord;
            }            
        }        
        if (i != K-1){
            curr_point = curr_point->next_point;
            curr_centroid->next_centroid = malloc(sizeof(struct centroids));
            assert(curr_centroid->next_centroid != NULL);
            curr_centroid = curr_centroid->next_centroid;
        }   
    }    
    curr_centroid->next_centroid = head_centroid;
    return head_centroid;
}

double euclid_dist(struct coord* data_point1, struct coord* data_point2){
    /* Calculate the euclidean distance between 2 points.
    Assuming they have the same number of coordinates. */
    int sum = 0;
    int curr_diff;
    while(data_point1 != NULL && data_point2 != NULL){        
        curr_diff = (data_point1->value) - (data_point2->value);
        sum += pow(curr_diff, 2);
        data_point1 = data_point1->next_coord;
        data_point2 = data_point2->next_coord;
    }
    return sqrt(sum);
}

void assign_to_cluster(struct data_points* point, struct centroids* head_centroid, int K){
    /* Assign a point to the closest cluster */
    struct centroids *curr_centroid, *min_cent;
    struct coord *cent_new_coord, *curr_pt_coord;
    double min_dist = __INT_MAX__;
    int i;
    int new_coord;

    double dist;
    curr_centroid = head_centroid;
    /* Find the closest centroid to the point. */
    for (i = 0; i < K; i++){
        dist = euclid_dist(point->coords, curr_centroid->coords);
        if (dist < min_dist){
            min_dist = dist;
            min_cent = curr_centroid;
        }
        curr_centroid = curr_centroid->next_centroid;
    }
    /* update field in point. */
    point->centroid = min_cent;
    /* Update fields in centroid (min_cent).
        add 1 to the cnt_points
        add the values of the point to the */
    min_cent->cnt_points +=1;
    curr_pt_coord = point->coords;
    cent_new_coord = min_cent->new_coords;
    while (curr_pt_coord != NULL){
        new_coord = curr_pt_coord->value;
        cent_new_coord->value += new_coord;
        curr_pt_coord = curr_pt_coord->next_coord;
        cent_new_coord = cent_new_coord->next_coord;
    }
    
}

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points){
    /* Assign each point to a cluster (centroid) */
    struct data_points *curr_point;    
    int i;
    curr_point = head_point;

    /*
    printf("%d\n", K);
    print_centroid(head_centroid);
    print_point(head_point);
    */
    for (i = 0; i < num_points; i++){  
        assign_to_cluster(curr_point, head_centroid, K); 
        curr_point = curr_point->next_point;
    }    
}

int update_centroids_and_check_covergence(struct centroids* cents, double eps, int K)
{
    struct coord* point_coords;
    struct coord* cent_coords;
    struct coord* prev_coords = malloc(sizeof(struct coord));
    struct coord* prev_head;
    int num_pts;
    double dist;
    int ret = 1;
    int i = 0;

    cent_coords = cents->coords;
    prev_head = prev_coords;

    while (i < coord_len)
        {
            prev_coords->next_coord = malloc(sizeof(struct coord));
            prev_coords = prev_coords->next_coord;
            i ++;
        }

    prev_coords->next_coord = NULL;
    prev_coords = prev_head;
    i = 0;

    while (i < K)
    {
        num_pts = cents->cnt_points;
        cents->cnt_points = 0;
        cent_coords = cents->coords;

        while (cent_coords != NULL)
        {
            prev_coords->value = cent_coords->value;
            prev_coords = prev_coords->next_coord;
            cent_coords = cent_coords->next_coord;
        }

        cent_coords = cents->coords;
        point_coords = cents->new_coords;
        prev_coords = prev_head;

        while (cent_coords != NULL)
        {
            cent_coords->value = (point_coords->value)/num_pts;
            point_coords->value = 0.0;
            cent_coords = cent_coords->next_coord;
            point_coords = point_coords->next_coord;
        }

        cent_coords = cents->coords;
        prev_coords = prev_head;

        dist = euclid_dist(cent_coords, prev_coords);
        if (dist >= eps)
        {
            ret = 0;
        }
        cents = cents->next_centroid;
        i ++;
    }
    prev_head = prev_coords;
    
    while (prev_coords != NULL)
    {
        prev_head = prev_head->next_coord;
        free(prev_coords);
        prev_coords = prev_head;
    }

    return ret;
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

void count(struct coord* coord)
{
    while (coord != NULL)
    {
        coord_len ++;
        coord = coord->next_coord;
    }
}


void run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points){
    double eps = 0.001;
    int conv_flag = 0;
    int i = 0;

    while ((i <= iter) && (conv_flag == 0))
    {
        print_centroids(head_centroid, K);
        assign_to_clusters(head_point, head_centroid, K, num_points);
        conv_flag = update_centroids_and_check_covergence(head_centroid, eps, K);
        i ++;
    }
    print_centroids(head_centroid, K);

    
}

int main(int argc, char **argv){    
    struct data_points *head_point;
    struct centroids *head_centroid;
    /*int points_cnt;*/
    int K;
    int iter;

    head_point = init_datapoints();
    count(head_point->coords);
    /*points_cnt = get_num_points(head_point);*/
    /*K = get_k(argv, points_cnt);*/
    K = get_k(argv, num_points);
    iter = get_iter(argv);
    if (argc != 3 || K == -1 || iter == -1){
        exit(0);
    }
    head_centroid = init_centroids(K, head_point);
    /*
    printf("The number of points is %d\n", points_cnt);
    print_point(head_point); 
    print_centroid(head_centroid);     
    */
    /*run_kmeans(head_point, head_centroid, K, iter, points_cnt);*/
    run_kmeans(head_point, head_centroid, K, iter, num_points);
    free_mem(head_point, head_centroid, num_points, K);
    return 0;

}