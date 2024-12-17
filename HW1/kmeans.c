# include <stdio.h>
# include <stdlib.h>


/* A value in a data point. next points to the next coordinate. */
struct coord
{
    double value;
    struct coord *next_coord;
};

/* A data point with values, linked list of coords. */
struct data_points
{
    struct data_points *next_point;
    struct coord *coords;
    int idx;
    struct centroids *centroid; /*We onlly need the one we point to, not the whole list. */
};

struct centroids
{
    struct centroids *next_centroid;
    struct coord *coords;
    int cnt_points; /* So we can calc the new mean */
    struct new_coords *new_coords; /* So we can add up coord and calc new mean */
};

void print_point(struct data_points *point);

int validate_input(int argc, char **argv, int N);

struct data_points* init_datapoints();

/* struct centroids init_centroids();*/

void assign_to_clusters();

void update_centroids();

int convergence();

int euclid_dist(struct coord data_point1, struct coord vetor2);

void run_kmenas();

void reset_centroids();

void print_point(struct data_points *point){
    struct coord *coord;
    coord = point->coords;
    printf("Printing point in index %d\n", point->idx);
    while (coord != NULL){
        printf("%f ",coord->value);
        coord = coord->next_coord;
    }
    printf("\n");
}

int validate_input(int argc, char **argv, int N){
    long K;
    long iter;

    if (argc != 3){
        return 0;
    }
    else{
         /* TODO: Handle exceptions */
        K = strtol(argv[1], NULL, 10);
        iter = strtol(argv[2], NULL, 10); 
        /* Check values */
        if (K <= 1 || K >= N){
            printf("Invalid number of clusters!");
            return 0;
        }
        if (iter <= 1 || iter >= 1000){
            printf("Invalid maximum iteration!");
            return 0;
        }
        printf("K = %ld\n", K);
        printf("iter = %ld\n", iter);    
    }
    return 1;
}

struct data_points* init_datapoints()
{
    /* Initialization */
    struct coord *head_coord, *curr_coord;
    struct data_points *head_point, *curr_point ;
    
    int rows = 0;
    double n;
    char c;

    head_coord = malloc(sizeof(struct coord));
    curr_coord = head_coord;
    curr_coord->next_coord = NULL;

    head_point = malloc(sizeof(struct data_points));
    head_point->idx = 0;
    curr_point = head_point;
    curr_point->next_point = NULL;

    while (scanf("%lf%c", &n, &c) == 2)
    {        
        /* last value in the coordinate- Files end with an empty line. */
        if (c == '\n')
        {        
            curr_coord->value = n;
            curr_point->coords = head_coord;
            curr_point->next_point = malloc(sizeof(struct data_points));
            curr_point = curr_point->next_point;
            curr_point->idx = rows;
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
    curr_point->next_point = head_point;    
    /* Returning the last point, so we can infer the anount of points.
        A better solution should be found TODO */   
    return curr_point;
}

int main(int argc, char **argv){    
    struct data_points *head_point;
    int points_cnt;
    int valid;
    /*
    int K;
    int iter;
    K = atoi (argv[0]);
    iter = atoi (argv[1]); */
    printf("Test\n");
    head_point = init_datapoints();
    /* Get the number of points N and then get the head point to be the one with idx 0*/
    points_cnt = head_point->idx +1;
    head_point = head_point->next_point;
    print_point(head_point);
    valid = validate_input(argc, argv, points_cnt);

    if (valid == 0){
        exit(0);
    }
    printf("The number of points is %d\n", points_cnt);
    return 0;

}