#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define N 50000000
#define MAX_CONST 10.1565

int main() {
    srand(time(NULL));

    double* nums = malloc(sizeof(double) * (N + 1));

    if (nums == NULL) {
        printf("Memory allocation failed");
        return 0;
    }

    for (int i = 0; i < N; i++) {
        nums[i] = rand() / (double)MAX_CONST;
    }

    double min_diff = fabs(nums[1] - nums[0]);

    #pragma omp parallel num_threads(4)
    {
        #pragma omp for reduction(min:min_diff)
        for (int i = 2; i < N; i++) {
            double diff = fabs(nums[i] - nums[i - 1]);

            if (diff < min_diff) {
                min_diff = diff;
            }
        }
    }

    printf("The minimum difference is %f \n", min_diff);

    return 1;
}