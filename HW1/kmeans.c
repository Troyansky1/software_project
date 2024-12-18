# include <stdio.h>
# include <stdlib.h>
# include <math.h>

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

void print_point(struct data_points *point);

void print_centroid(struct centroids *centroid);

int get_k(char **argv, int N);

int get_iter(char **argv);

struct data_points* init_datapoints();

struct centroids* init_centroids(int K, struct data_points* data_point);

double euclid_dist(struct coord* data_point1, struct coord* data_point2);

void assign_to_cluster(struct data_points* point, struct centroids* head_centroid, int K);

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points);

void update_centroids();

int convergence();

void run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points);

void reset_centroids();

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

void print_centroid(struct centroids *centroid){
    /* Debug printint of a centroid. */
    struct coord *coord;
    coord = centroid->coords;
    printf("Num of points in centroid is %d\n", centroid->cnt_points);
    printf("Printing centroid values:\n");    
    while (coord != NULL){
        printf("%f ",coord->value);
        coord = coord->next_coord;
    }
    printf("\n");
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
        printf("Invalid number of clusters!");
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

    head_coord = malloc(sizeof(struct coord));
    curr_coord = head_coord;
    curr_coord->next_coord = NULL;

    head_point = malloc(sizeof(struct data_points));
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
            curr_point = curr_point->next_point;   
            curr_point->idx = -1;         
            curr_point->next_point = NULL;
            head_coord = malloc(sizeof(struct coord));
            curr_coord = head_coord;
            curr_coord->next_coord = NULL;
            rows ++;
            continue;
        }
        /* Add another value to same coordinate */
        curr_coord->value = n;
        curr_coord->next_coord = malloc(sizeof(struct coord));
        curr_coord = curr_coord->next_coord;
        curr_coord->next_coord = NULL;        
    }
    curr_point = NULL;    
    /* Returning the last point, so we can infer the anount of points.
        A better solution should be found TODO */   
    return head_point;
}

struct centroids* init_centroids(int K, struct data_points* data_point){
    /* Initialize centroids with values of the first K data points. */
    struct data_points *curr_point;
    struct centroids *head_centroid, *curr_centroid;
    struct coord *head_coord, *curr_cent_coord, *curr_pt_coord;
    
    int i;
    curr_point = data_point;
    head_centroid = malloc(sizeof(struct centroids));
    curr_centroid = head_centroid;
    for (i = 0; i < K; i++){
        /* At initiation, there are no points allocated to any centroid, and no need to update the coordinations.*/
        curr_centroid->cnt_points = 0;
        curr_centroid->new_coords = NULL;        
        /*TODO: Copy value of coords from the first K points to the K centroids. */
        head_coord = malloc(sizeof(struct coord));
        curr_cent_coord = head_coord;  
        curr_pt_coord = curr_point->coords;
        curr_centroid->coords = head_coord;    
        /* Copy each value in the coordinate*/  
        while(curr_pt_coord != NULL){                        
            curr_cent_coord->value = curr_pt_coord->value;     
            curr_pt_coord = curr_pt_coord->next_coord;
            /*TODO: I'm not sure this is the most elegant way to do this */
            if (curr_pt_coord == NULL){
                curr_cent_coord->next_coord = NULL;
            }
            else{
                curr_cent_coord->next_coord = malloc(sizeof(struct coord));
                curr_cent_coord = curr_cent_coord->next_coord;
            }            
        }        
        if (i != K-1){
            curr_point = curr_point->next_point;
            curr_centroid->next_centroid = malloc(sizeof(struct centroids));
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
        printf("--");
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
    double dist;
    printf("--");
    curr_centroid = head_centroid;
    /* Find the closest centroid to the point. */
    printf("--");
    for (i = 0; i < K; i++){
        printf("--");
        dist = euclid_dist(point->coords, curr_centroid->coords);
        printf("--");
        if (dist < min_dist){
            min_dist = dist;
            min_cent = curr_centroid;
        }
        curr_centroid = curr_centroid->next_centroid;
        printf("--");
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
        cent_new_coord->value += curr_pt_coord->value;
        curr_pt_coord = curr_pt_coord->next_coord;
        cent_new_coord = cent_new_coord->next_coord;
    }
    
}

void assign_to_clusters(struct data_points* head_point, struct centroids* head_centroid, int K, int num_points){
    /* Assign each point to a cluster (centroid) */
    struct centroids *curr_centroid, *min_cent;
    double min_dist = __INT_MAX__;
    int j;
    double dist;

    struct data_points *curr_point;    
    int i;
    curr_point = head_point;

    curr_centroid = head_centroid;
    /*
    printf("%d\n", K);
    print_centroid(head_centroid);
    print_point(head_point);
    */
    for (i = 0; i < num_points; i++){    
        printf("i = %d\n", i); 
        for (j = 0; j < K; j++){
            printf("j = %d\n", j); 
            dist = euclid_dist(curr_point->coords, curr_centroid->coords);
            if (dist < min_dist){
                min_dist = dist;
                min_cent = curr_centroid;
                print_centroid(min_cent);
            }
            curr_centroid = curr_centroid->next_centroid;
        }   
        /* assign_to_cluster(curr_point, head_centroid, K); */
        curr_point = curr_point->next_point;
    }    
}

void run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points){
    /* int eps = 0.001; */
    printf("Iter %d\n", iter);
    /* Iterate iter times TODO*/
    assign_to_clusters(head_point, head_centroid, K, num_points);
}

int main(int argc, char **argv){    
    struct data_points *head_point;
    struct centroids *head_centroid;
    int points_cnt;
    int K;
    int iter;

    head_point = init_datapoints();
    points_cnt = get_num_points(head_point);
    K = get_k(argv, points_cnt);
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
    run_kmeans(head_point, head_centroid, K, iter, points_cnt);
    return 0;

}