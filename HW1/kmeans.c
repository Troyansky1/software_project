# include <stdio.h>
# include <stdlib.h>

int validate_input(int argc, char **argv);

void init_datapoints(int argc, char **argv);

void assign_to_clusters();

void update_centroids();

int convergence();

int euclid_dist(coord vector1, coord vetor2);

void run_kmenas();


struct coord
{
    double value;
    struct coord *next;
};
struct vector
{
    struct vector *next;
    struct coord *coords;
};

int main(int argc, char **argv)
{

    struct vector *head_vec, *curr_vec, *next_vec;
    struct coord *head_coord, *curr_coord, *next_coord;
    int i, j, rows = 0, cols;
    double n;
    char c;

    head_coord = malloc(sizeof(struct coord));
    curr_coord = head_coord;
    curr_coord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;


    while (scanf("%lf%c", &n, &c) == 2)
    {

        if (c == '\n')
        {
            curr_coord->value = n;
            curr_vec->coords = head_coord;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_coord = malloc(sizeof(struct coord));
            curr_coord = head_coord;
            curr_coord->next = NULL;
            continue;
        }

        curr_coord->value = n;
        curr_coord->next = malloc(sizeof(struct coord));
        curr_coord = curr_coord->next;
        curr_coord->next = NULL;
    }

    return 0;
}

