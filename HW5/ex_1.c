#include <stdio.h>
#include <stdlib.h>   
#include <time.h>    
#include <omp.h>

typedef struct {
    int request_id;
    int user_id;
    int response_time_ms;
} variables;

int main() {
    srand(time(NULL));

    variables* arr = malloc(20000 * sizeof(variables) + 1); 



    if (arr == NULL) {
        printf("Memory allocation failed");
        return 0;
    }

    int num_of_fast[4] = {0};
    int num_of_medium[4] = {0};
    int num_of_slow[4] = {0};
    
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();

        #pragma omp single
        {
            for (int i = 0; i < 20000 - 1; i++) {
                int request_id = i;                           // I don't see the meaning of this variable so it is denoted i 
                int user_id = omp_get_thread_num();           // Same for this one, randomly the id of thread
                int ms = rand() % 500;

                arr[i] = (variables){request_id, user_id, ms};
            }
        }

        #pragma omp barrier                                   // No need for this as single has implicit barrier

        #pragma omp for
        for (int i = 0; i < 20000 - 1; i++) {
            if (arr[i].response_time_ms < 100) {
                num_of_fast[thread_id]++;
            } else if (arr[i].response_time_ms >= 100 && arr[i].response_time_ms < 300) {
                num_of_medium[thread_id]++;
            } else {
                num_of_slow[thread_id]++;
            }
        }

        #pragma omp barrier                                   // Again, no need for the barrier, there is an implicit one in for

        #pragma omp single
        {
            int total_fast = 0, total_medium = 0, total_slow = 0;

            for (int i = 0; i < 4; i++) {
                total_fast += num_of_fast[i];
                total_medium += num_of_medium[i];
                total_slow += num_of_slow[i];
            }

            printf("Number of fast logs: %d\n", total_fast);
            printf("Number of medium logs: %d\n", total_medium);
            printf("Number of slow logs: %d\n", total_slow);
        }
    }

    free(arr);
    return 1;
}