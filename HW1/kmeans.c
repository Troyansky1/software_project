# include <stdio.h>
# include <stdlib.h>
# include <math.h>

#ifdef DEBUG
#define DPRINT printf("Debug print at %d\n", __LINE__)
#else
#define DPRINT {}
#endif

#define ITERNUM 200
#define EPS 0.001

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
    double cnt_points; /* So we can calc the new mean */
    struct coord *new_coords; /* So we can add up coord and calc new mean */
};

int num_points;

int coord_len;

void print_point(struct data_points *point);

void print_centroids(struct centroids *head_centroid, int K);

int get_k(char **argv, int N);

int get_iter(char **argv, int argc);

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
            printf("%.4f,", curr->value);
            curr = curr->next_coord;
        }
        printf("%.4f\n", curr->value);
        head_centroid = head_centroid->next_centroid;
        i ++;
    }
}


int get_k(char **argv, int N){
    /* Validates the value of N as received in argv.
    valid values:
    1 < K < N, K is a natural number.
    */
    long K;
    double K_d;
    char *endptr;
    if (argv[1] == NULL) {
        printf("Invalid number of clusters!\n");
        return -1;
    }
    K_d = strtod(argv[1], &endptr);
    /* Check if strtol failed- K not a natural number */
    if (*endptr != '\0') {
        printf("Invalid number of clusters!\n");
        return -1;
    }

    /* Check if the value is a whole number */
    if (K_d != (long)K_d) {
        printf("Invalid number of clusters!\n");
        return -1;
    }

    /* Convert to long for range checking */
    K = (long)K_d;

    /* Check values */
    if (K <= 1 || K >= N){
        printf("Invalid number of clusters!\n");
        return -1;
    }
    return (int)K;
}


int get_iter(char **argv, int argc){
    /* Validates the value of iter as received in argv.
    valid values:
    1 < iter < 1000, iter is a natural number.
    */
    long iter;
    double iter_d;
    char *endptr;
    /* If the number of iterations is not specified, return the default value. */
    if (argc <= 2)
    {
        return ITERNUM;
    }

    iter_d = strtod(argv[2], &endptr); 
    /* Check for conversion errors */
    if (*endptr != '\0') {
        printf("Invalid maximum iteration!");
        return -1;
    }
    
    /* Check if the value is a whole number */
    if (iter_d != (long)iter_d) {
        printf("Invalid maximum iteration!");
        return -1;
    }

    /* Convert to long for range checking */
    iter = (long)iter_d;

    /* Check values */
    if (iter <= 1 || iter >= 1000){
        printf("Invalid maximum iteration!");
        return -1;
    }   
    return (int)iter;
}



struct data_points* init_datapoints()
{
    /* Initialization- code taken from lecture notes. */
    struct coord *head_coord, *curr_coord;
    struct data_points *head_point, *curr_point ;
    
    double n;
    char c;
    int cnt = 0;

    head_coord = malloc(sizeof(struct coord));
    if (head_coord == NULL) {
        fprintf(stderr, "An error has accured\n");
        return NULL; 
    }
    curr_coord = head_coord;
    curr_coord->next_coord = NULL;

    head_point = malloc(sizeof(struct data_points));
    if (head_point == NULL) {
        fprintf(stderr, "An error has accured\n");
        free(head_coord); 
        return NULL; 
    }
    curr_point = head_point;
    curr_point->next_point = NULL;

    while (scanf("%lf%c", &n, &c) == 2)
    {        
        /* last value in the coordinate- Files end with an empty line. */
        if (c == '\n')
        {        
            curr_coord->value = n;
            curr_point->coords = head_coord;
            curr_point->idx = cnt;
            curr_point->next_point = malloc(sizeof(struct data_points));     
            if (curr_point->next_point == NULL) {
                fprintf(stderr, "An error has accured\n");
                free_points(head_point, cnt); 
                return NULL; 
            }
            curr_point = curr_point->next_point;   
            curr_point->idx = -1;         
            curr_point->next_point = NULL;
            head_coord = malloc(sizeof(struct coord));
            if (head_coord == NULL) {
                fprintf(stderr, "An error has accured\n");
                free_points(head_point, cnt); 
                return NULL; 
            }
            curr_coord = head_coord;
            curr_coord->next_coord = NULL;
            cnt ++;
            continue;
        }
        /* Add another value to same coordinate */
        curr_coord->value = n;
        curr_coord->next_coord = malloc(sizeof(struct coord));
        if (curr_coord->next_coord == NULL) {
            fprintf(stderr, "An error has accured\n");
            free_points(head_point, cnt); 
            return NULL; 
        }
        curr_coord = curr_coord->next_coord;
        curr_coord->next_coord = NULL;        
    }
    curr_point = NULL;  
    num_points = cnt;  
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
        fprintf(stderr, "An error has accured\n");
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
            fprintf(stderr, "An error has accured\n");
            free_centroids(head_centroid, i); 
            return NULL;
        }
        curr_new_coord = head_new_coords;
        curr_centroid->new_coords = head_new_coords;

        /*Copy value of coords from the first K points to the K centroids. */
        head_coord = malloc(sizeof(struct coord));
        if (head_coord == NULL) {
            fprintf(stderr, "An error has accured\n");            
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
                    fprintf(stderr, "An error has accured\n");
                    free_centroids(head_centroid, i); 
                    return NULL; 
                }
                curr_cent_coord = curr_cent_coord->next_coord;

                curr_new_coord->next_coord = malloc(sizeof(struct coord));
                if (curr_new_coord->next_coord == NULL) {
                    fprintf(stderr, "An error has accured\n");
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
                fprintf(stderr, "An error has accured\n");
                free_centroids(head_centroid, i);         
                return NULL;
            }
            curr_centroid = curr_centroid->next_centroid;
        }   
    }    
    curr_centroid->next_centroid = head_centroid;
    return head_centroid;
}

double euclid_dist(struct coord* data_point1, struct coord* data_point2){
    /* Calculate the euclidean distance between 2 points.
    Assuming they have the same number of coordinates. */
    double sum = 0;
    double curr_diff;
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
    double new_coord;

    double dist;
    curr_centroid = head_centroid;
    /* Find the closest centroid to the point. */
    for (i = 0; i < K; i++){
        dist = euclid_dist(point->coords, curr_centroid->coords);
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
    int i;
    int j;

    cent_coords = cents->coords;
    prev_head = prev_coords;

    for (i = 0; i < coord_len; i++){
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
        while (cent_coords != NULL)
        {
            prev_coords->value = cent_coords->value;
            prev_coords = prev_coords->next_coord;
            cent_coords = cent_coords->next_coord;
        }

        cent_coords = cents->coords;
        point_coords = cents->new_coords;
        prev_coords = prev_head;
        /* Iterate over each coordinate and update its value as the mean of the points in its cluster.*/
        while (cent_coords != NULL)
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

        dist = euclid_dist(cent_coords, prev_coords);
        if (dist >= eps)
        {
            ret = 0;
        }
        cents = cents->next_centroid;        
    }
    prev_head = prev_coords;
    free_coords(prev_head);
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
    int conv_flag = 0;
    int i = 0;

    while ((i <= iter) && (conv_flag == 0))
    {
        assign_to_clusters(head_point, head_centroid, K, num_points);
        conv_flag = update_centroids_and_check_covergence(head_centroid, EPS, K);
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
    if (head_point == NULL){
        return 1;
    }
    count(head_point->coords);
    K = get_k(argv, num_points);
    iter = get_iter(argv, argc);
    if (argc > 3 || argc < 2 || K == -1 || iter == -1){
        return 1;
    }
    head_centroid = init_centroids(K, head_point);
    if (head_centroid == NULL){
        free_points(head_point, num_points);
        return 1;
    }
    run_kmeans(head_point, head_centroid, K, iter, num_points);
    free_mem(head_point, head_centroid, num_points, K);
    return 0;

}