#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define N 10000


int main() {
    srand(time(NULL));

    int* nums = malloc(sizeof(int) * (N + 1));

    if (nums == NULL) {
        printf("Memory allocation failed");
        return 0;
    }

    for (int i = 0; i < N; i++) {
        nums[i] = rand() % 1000;
    }

    int max_val = nums[0];

    #pragma omp parallel for num_threads(4) reduction(max:max_val)
    for (int i = 1; i < N; i++) {
        int val = nums[i];

        if (val > max_val) {
            max_val = val;
        }
    }

    float threshold = 0.8 * max_val;
    int sum = 0;

    #pragma omp parallel for num_threads(4) reduction(+:sum)
    for (int i = 0; i < N; i++) {
        if (nums[i] > threshold) {
            sum += nums[i];
        }
    }

    printf("The threshold is %f and the sum of numbers above it is %d \n", threshold, sum);

    return 1;
}