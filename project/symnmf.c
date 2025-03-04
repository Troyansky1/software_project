# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "symnmf.h"



float** init_matrix_mem(int n) {
    float *p;
    float **a;
    int i;
    /* Allocate contiguous memory for the matrix */
    p = calloc(n * n, sizeof(float));  
    if (!p) return NULL;  
    
    a = calloc(n, sizeof(float*));  
    if (!a) {
        free(p);
        return NULL;
    }

    for (i = 0; i < n; i++)
        a[i] = p + i * n;

    return a;
}

void free_matrix_mem(float** a) {
    if (a) {
        /* Free the contiguous block of memory */
        free(a[0]);  
        /*  Free the row pointers */
        free(a);     
    }
}

void get_matrix_params(int *n, int *d, FILE* file) {
    char ch;
    int first_row = 1;
    *n = 0;
    *d = 1; 

    while ((ch = fgetc(file)) != EOF) {
        if (first_row) {
            if (ch == ',') {
                (*d)++; 
            }
        }
        if (ch == '\n') {
            (*n)++; 
            first_row = 0;
        }
    }

    /* handle case of file not ending with new line
    if (ch != '\n' && d > 0) {
        n++;
    } */
    printf("The num of lines is: %d\n", *n);
    printf("The num of rows is: %d\n", *d);
    rewind(file);
}

void init_X(FILE* file, float** X, int n){
    /* Initialize X with values*/
    int i;
    int j;
    /* Buffer to hold each line TODO handle bigger lengths*/
    char line[1024];  
    char *ptr;  
    for (i = 0; i < n; i++){
        fgets(line, sizeof(line), file);
        ptr = line;
        j = 0;
        /*  Parse the comma-separated values in the line */
        while (*ptr != '\0') {
            if (sscanf(ptr, "%f", &X[i][j]) == 1){
                while (*ptr != ',' && *ptr != '\0') {
                    ptr++;
                }
                if (*ptr == ',') {
                    ptr++; 
                    j++;
                }
            }
            else{
                printf("An Error Has Occurred\n");
                free_matrix_mem(X);
                return; 
            }
        }
    }
}


float** create_X(FILE* file){
    int n;
    int d;
    float** X;
    get_matrix_params(&n, &d, file);
    if (n <= 0 || d <= 0){
        printf("An Error Has Occurred\n");
        return NULL; 
    }
    X = init_matrix_mem(n);
    init_X(file, X, n);
    return X;
}

void calc_euclid_dist();

float** calc_similarity(float **X);

void calc_similarity_matrix();

void calc_diag_deg();

void calc_diag_deg_matrix();

void calc_inv_sqrt();

void mat_mult();

void calc_norm_sim_matrix();

void calc_m();

void init_H();

void update_H();

void check_convergence();

void optimize_H();

void derive_clustering_sol();

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1],"r");
    if (argc != 2){
        printf("An Error Has Occurred\n");
        return 0;
    }
    if (fp == NULL){
        printf("An Error Has Occurred\n");
        return 0;
    }
    create_X(fp);
    return 1;
}