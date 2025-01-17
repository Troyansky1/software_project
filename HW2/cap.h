# ifndef CAP_H_
# define CAP_H_

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
    struct centroids *centroid; /*We only need the one we point to, not the whole list. */
};

/* A centroid with values, cyclyc linked list of centroids. */
struct centroids
{
    struct centroids *next_centroid;
    struct coord *coords;
    double cnt_points; /* So we can calc the new mean */
    struct coord *new_coords; /* So we can add up coord and calc new mean */
};

void free_coords(struct coord* head_coord);

void free_points(struct data_points* head_point, int N);

void free_centroids(struct centroids* head_centroid, int K);

void free_mem(struct data_points* head_point, struct centroids* head_centroid, int N, int K);

void run_kmeans(struct data_points* head_point, struct centroids* head_centroid, int K, int iter, int num_points);

static PyObject* GetCoordsList(struct coord *head_coord, int dim);

static PyObject* GetCentsList(struct centroids *head_centroid, int dim, int K);

struct data_points *init_datapoints(PyObject *dpts, int N, int dim);

static PyObject* GetCentsList(struct centroids *head_centroid, int dim, int K);

void print_centroids(struct centroids *head_centroid, int K);

# endif