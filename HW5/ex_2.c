#include <stdio.h>
#include <stdlib.h>   
#include <time.h>    
#include <omp.h>

typedef struct {
    int order_id;
    int distance_km;
    char priority;
} variables;

int main() {
    srand(time(NULL));

    variables* arr = malloc(10000 * sizeof(variables) + 1); 

    if (arr == NULL) {
        printf("Memory allocation failed");
        return 0;
    } 

    int thread_high_count[4] = {0};
    int max_distance;

    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();

        #pragma omp single
        {
            max_distance = 30;
        }

        #pragma omp for
        for (int i = 0; i < 10000 - 1; i++) {
            int distance = rand() % 80;

            arr[i].order_id = i;
            arr[i].distance_km = distance;

            if (distance > max_distance) {
                arr[i].priority = 'L';
            } else {
                arr[i].priority = 'H';
            }
        }

        #pragma omp barrier                                   

        #pragma omp single
        {
            printf("All priorities have been asigned \n");
        }

        #pragma omp for
        for (int i = 0; i < 10000; i++) {
            if (arr[i].priority == 'H') {
                thread_high_count[thread_id]++;
            }
        }

        #pragma omp barrier

        #pragma omp single
        {
            int count = 0;

            for (int i = 0; i < 4; i++) {
                count += thread_high_count[i];
            }

            printf("The number of high priority orders is %d \n", count);
        }
    }

    free(arr);
    return 1;
}