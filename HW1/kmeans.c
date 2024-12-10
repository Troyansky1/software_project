# include <stdio.h>
# include <stdlib.h>

int validate_input(int argc, char **argv);

void init_datapoints(int argc, char **argv, int k);

void assign_to_clusters();

void update_centroids();

int convergence();

int euclid_dist(coord data_point1, coord vetor2);

void run_kmenas();

void reset_centroids();



// A value in a data point. next points to the next coordinate.
struct coord
{
    double value;
    struct coord *next;
};

// A data point with values, linked list of coords. 
struct data_points
{
    struct data_points *next;
    struct coord *coords;
    struct centroids *centroid; //We onlly need the one we point to, not the whole list.
};

struct centroids
{
    struct centroids *next;
    struct coord *coords;
    int cnt_points; // So we can calc the new mean
    struct new_coords; // So we can add up coord and calc new mean
}


int init_datapoints(int argc, char **argv, int k)
{
    // Initialization
    struct data_points *head_point, *curr_point, *next_point;
    struct coord *head_coord, *curr_coord, *next_coord;
    int i, j = 0, rows = 0, cols = 0;
    double n;
    char c;

    head_coord = malloc(sizeof(struct coord));
    curr_coord = head_coord;
    curr_coord->next = NULL;

    head_point = malloc(sizeof(struct data_point));
    curr_point = head_point;
    curr_point->next = NULL;

    // Read input
    while (scanf("%lf%c", &n, &c) == 2)
    {
        // last value in the coordinate. 
        if (c == '\n')
        {
            curr_coord->value = n;
            curr_point->coords = head_coord;
            curr_point->next = malloc(sizeof(struct data_point));
            curr_point = curr_point->next;
            curr_point->next = NULL;
            head_coord = malloc(sizeof(struct coord));
            curr_coord = head_coord;
            curr_coord->next = NULL;
            rows ++;
            continue;
        }
        // Add another value to same coordinate
        curr_coord->value = n;
        curr_coord->next = malloc(sizeof(struct coord));
        curr_coord = curr_coord->next;
        curr_coord->next = NULL;
        j++
    }
    cols = j / rows; // Calculate the num of columns as the total coords/rows
    return 0;
}

