#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Exercise 1

void* thread1Function(void* arg) {
    pthread_t self = pthread_self();
    printf("The address of the first thread is %lu \n", (unsigned long)self);
}

void* thread2Function(void* arg) {
    pthread_t self = pthread_self();
    printf("The address of the second thread is %lu \n", (unsigned long)self);
}

void* thread3Function(void* arg) {
    pthread_t self = pthread_self();
    printf("The address of the third thread is %lu \n", (unsigned long)self);
}

void ex1() {
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    pthread_create(&thread1, NULL, thread1Function, NULL);
    pthread_create(&thread2, NULL, thread2Function, NULL);
    pthread_create(&thread3, NULL, thread3Function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
}

// Exercise 2

#define SIZE 50000000   // 5 million
#define N 4

typedef struct {
    int *arr;
    long start;
    long end;
    int partial_sum;
} ThreadArgs2;

// Thread function
void* sum_part(void* arg) {
    ThreadArgs2* data = (ThreadArgs2*)arg;

    data->partial_sum = 0;

    for (long i = data->start; i < data->end; i++) {
        data->partial_sum += data->arr[i];
    }

    return NULL;
}

void ex2() { 

    int *arr = malloc(SIZE * sizeof(int));
    srand(time(NULL));

    for (long i = 0; i < SIZE; i++) {
        arr[i] = rand() % 10 + 1;
    }

    // Sequentially 

    clock_t start_seq = clock();

    int seq_sum = 0;
    for (long i = 0; i < SIZE; i++) {
        seq_sum += arr[i];
    }

    clock_t end_seq = clock();

    double time_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;

    printf("Sequential sum: %d\n", seq_sum);
    printf("Sequential time: %.3f seconds\n\n", time_seq);

    // In Parallel

    pthread_t *threads = malloc(N * sizeof(pthread_t));
    ThreadArgs2 *thread_data = malloc(N * sizeof(ThreadArgs2));

    long chunk = SIZE / N;

    clock_t start_par = clock();

    for (int i = 0; i < N; i++) {

        thread_data[i].arr = arr;
        thread_data[i].start = i * chunk;

        if (i == N - 1) {
            thread_data[i].end = SIZE;
        } else {
            thread_data[i].end = (i + 1) * chunk;
        }

        pthread_create(&threads[i], NULL, sum_part, &thread_data[i]);
    }

    int total_sum = 0;

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }

    clock_t end_par = clock();
    double time_par = (double)(end_par - start_par) / CLOCKS_PER_SEC;

    printf("Parallel sum: %d\n", total_sum);
    printf("Parallel time with %d threads: %.3f seconds\n", N, time_par);

    free(arr);
    free(threads);
    free(thread_data);
}

// Exercise 3

typedef struct {
    int *arr;
    int start;
    int end;
    int max;
} ThreadArgs3;

void* findingMax(void* args) {
    ThreadArgs3* data = (ThreadArgs3*)args;

    data->max = data->arr[data->start];

    for (int i = data->start + 1; i < data->end; i++) {
        if (data->arr[i] > data->max) {
            data->max = data->arr[i];
        }
    }

    return NULL;
}

void ex3() {
    int* arr = malloc(SIZE * sizeof(int));
    srand(time(NULL));
    
    for (long i = 0; i < SIZE; i++) {
        arr[i] = rand() % 10000 + 1;
    }

    // Sequentially

    clock_t start_seq = clock();

    int max = arr[0];

    for (long i = 1; i < SIZE; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    clock_t end_seq = clock();
    double time_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;

    printf("Sequential max: %d\n", max);
    printf("Sequential time: %.3f seconds\n\n", time_seq);

    // In parallel

    clock_t start_par = clock();

    pthread_t threads[N];
    ThreadArgs3 thread_data[N];

    long chunk = SIZE / N;

    for (int i = 0; i < N; i++) {
        thread_data[i].arr = arr;
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == N - 1) ? SIZE : (i + 1) * chunk;

        pthread_create(&threads[i], NULL, findingMax, &thread_data[i]);
    }

    int global_max = thread_data[0].max;

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
        if (thread_data[i].max > global_max) {
            global_max = thread_data[i].max;
        }
    }

    clock_t end_par = clock();
    double time_par = (double)(end_par - start_par) / CLOCKS_PER_SEC;

    printf("Parallel max: %d\n", global_max);
    printf("Parallel time with %d threads: %.3f seconds\n", N, time_par);

    free(arr);
}

// Exercise 4

#define EX4_SIZE 1000000
#define X 10

typedef struct {
    int start;
    int end;
    int count;
} ThreadArgs4;

void* findingPrime(void* args) {
    ThreadArgs4* data = (ThreadArgs4*)args;
    data->count = 0;

    for (int i = data->start; i < data->end; i++) {
        if (i <= 1) continue;

        int prime = 1;

        for (int j = 2; j <= (int)sqrt(i); j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }

        if (prime == 0) continue;

        data->count++;
    }

    return NULL;
}

void ex4() {
    clock_t start_seq = clock();

    int count = 0;

    for (int i = 2; i < EX4_SIZE; i++) {
        int prime = 1;

        for (int j = 2; j <= (int)sqrt(i); j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }

        if (prime == 0) continue;

        count++;
    }

    clock_t end_seq = clock();
    double time_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;

    printf("Sequential count: %d\n", count);
    printf("Sequential time: %.3f seconds\n\n", time_seq);

    clock_t start_par = clock();

    pthread_t threads[X];
    ThreadArgs4 thread_data[X];

    int chunk = EX4_SIZE / X;

    for (int i = 0; i < X; i++) {
        thread_data[i].start = i * chunk;
        thread_data[i].end = (i == X - 1) ? EX4_SIZE : (i + 1) * chunk;

        pthread_create(&threads[i], NULL, findingPrime, &thread_data[i]);
    }

    int final_count = 0;

    for (int i = 0; i < X; i++) {
        pthread_join(threads[i], NULL);
        final_count += thread_data[i].count;
    }

    clock_t end_par = clock();
    double time_par = (double)(end_par - start_par) / CLOCKS_PER_SEC;

    printf("Parallel count: %d\n", final_count);
    printf("Parallel time with %d threads: %.3f seconds\n", X, time_par);
}

void main() {
    //ex1();
    //ex2();
    //ex3();
    ex4();
}

