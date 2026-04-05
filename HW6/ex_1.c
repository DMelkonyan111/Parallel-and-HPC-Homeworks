#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 100000000

int main() {
    srand(time(NULL));

    int* arr = malloc(sizeof(int) * N + 1);

    if (arr == NULL) {
        printf("Memory allocation failed");
        return 0;
    }

    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 256;
    }

    int hist_naive[256] = {0};
    int hist_critical[256] = {0};
    int hist_reduction[256] = {0};

    double start_naive = omp_get_wtime();

    #pragma omp parallel num_threads(4)     // Naive version
    {
        #pragma omp for 
        for (int i = 0; i < N; i++) {
            hist_naive[arr[i]]++;
        }
    }

    double end_naive = omp_get_wtime();
    double start_critical = omp_get_wtime();

    #pragma omp parallel for num_threads(4)  // Critical syntax implementation  
    for (int i = 0; i < N; i++) {
        #pragma omp critical
        {   
            int val = arr[i];
            hist_critical[val]++;
        }
    }

    double end_critical = omp_get_wtime();
    double start_reduction = omp_get_wtime();

    #pragma omp parallel for num_threads(4) reduction(+:hist_reduction[:256])
    for (int i = 0; i < N; i++) {
        int val = arr[i];
        hist_reduction[val]++;
    }
    
    double end_reduction = omp_get_wtime();

    printf("Here are the execution times of 3 versions \n");
    printf("Naive: %f \n", end_naive - start_naive);
    printf("Critical: %f \n", end_critical - start_critical);
    printf("Reduction: %f \n", end_reduction - start_reduction);

    free(arr);

    return 1;
}