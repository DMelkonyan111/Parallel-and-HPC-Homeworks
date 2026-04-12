#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define THREAD_NUM 4

int fib_recursion(int num) {
    if (num <= 2) {
        return 1;
    }

    int x, y;

    #pragma omp task shared(x)
    x = fib_recursion(num - 1);

    #pragma omp task shared(y)
    y = fib_recursion(num - 2);

    #pragma omp taskwait

    return x + y;
}

int fib(int num) {
    // Base cases
    if (num <= 2) {
        return 1;
    }

    // Sequential implementation for small num
    if (num <= 10) {
        int x = 1, y = 1;
        int z = 0;

        for (int i = 3; i <= num; i++) {
            z = x + y;

            x = y;
            y = z;
        }

        return z;
    }

    // Parallelized version
    int result = 0;

    #pragma omp parallel num_threads(THREAD_NUM)
    {
        #pragma omp single
        {
            result = fib_recursion(num);
        }
    }

    return result;
}

int main() {
    int num = 25;
    int fib_num = fib(num);

    printf("The fibonacci number at %d's position is %d \n", num, fib_num);
}
