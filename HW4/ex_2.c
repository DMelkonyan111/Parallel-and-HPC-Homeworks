#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <immintrin.h>

#define N (100 * 1024 * 1024)
#define THREADS 4

typedef struct {
    char *buffer;
    int start;
    int end;
} thread_args;

pthread_mutex_t lock;
char *buffer_mt;
char *buffer_simd;
char *buffer_simd_mt;

void *thread_convert(void *args)
{
    thread_args *data = (thread_args *)args;
    for (int i = data->start; i < data->end; i++)
    {
        if (data->buffer[i] >= 'a' && data->buffer[i] <= 'z')
            data->buffer[i] -= 32;
    }
    return NULL;
}

void only_threads()
{
    pthread_t t[THREADS];
    int chunk = N / THREADS;
    thread_args args[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        args[i].buffer = buffer_mt;
        args[i].start = i * chunk;
        args[i].end = (i == THREADS - 1) ? N : (i + 1) * chunk;
        pthread_create(&t[i], NULL, thread_convert, &args[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);
}

void only_simd()
{
    int i = 0;
    for (; i <= N - 32; i += 32)
    {
        __m256i vec = _mm256_loadu_si256((__m256i *)(buffer_simd + i));
        __m256i a = _mm256_set1_epi8('a');
        __m256i z = _mm256_set1_epi8('z');
        __m256i mask = _mm256_and_si256(_mm256_cmpgt_epi8(vec, _mm256_sub_epi8(a, _mm256_set1_epi8(1))),
                                        _mm256_cmpgt_epi8(_mm256_add_epi8(z, _mm256_set1_epi8(1)), vec));
        __m256i offset = _mm256_set1_epi8(32);
        vec = _mm256_sub_epi8(vec, _mm256_and_si256(mask, offset));
        _mm256_storeu_si256((__m256i *)(buffer_simd + i), vec);
    }
    for (; i < N; i++)
    {
        if (buffer_simd[i] >= 'a' && buffer_simd[i] <= 'z')
            buffer_simd[i] -= 32;
    }
}

void *simd_thread(void *arg)
{
    thread_args *data = (thread_args *)arg;
    int i = data->start;
    for (; i <= data->end - 32; i += 32)
    {
        __m256i vec = _mm256_loadu_si256((__m256i *)(data->buffer + i));
        __m256i a = _mm256_set1_epi8('a');
        __m256i z = _mm256_set1_epi8('z');
        __m256i mask = _mm256_and_si256(_mm256_cmpgt_epi8(vec, _mm256_sub_epi8(a, _mm256_set1_epi8(1))),
                                        _mm256_cmpgt_epi8(_mm256_add_epi8(z, _mm256_set1_epi8(1)), vec));
        __m256i offset = _mm256_set1_epi8(32);
        vec = _mm256_sub_epi8(vec, _mm256_and_si256(mask, offset));
        _mm256_storeu_si256((__m256i *)(data->buffer + i), vec);
    }
    for (; i < data->end; i++)
    {
        if (data->buffer[i] >= 'a' && data->buffer[i] <= 'z')
            data->buffer[i] -= 32;
    }
    return NULL;
}

void simd_plus_threads()
{
    pthread_t t[THREADS];
    int chunk = N / THREADS;
    thread_args args[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        args[i].buffer = buffer_simd_mt;
        args[i].start = i * chunk;
        args[i].end = (i == THREADS - 1) ? N : (i + 1) * chunk;
        pthread_create(&t[i], NULL, simd_thread, &args[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);
}

void main()
{
    srand(time(NULL));

    buffer_mt = malloc(N);
    buffer_simd = malloc(N);
    buffer_simd_mt = malloc(N);

    for (int i = 0; i < N; i++)
    {
        int r = rand() % 95 + 32;
        buffer_mt[i] = buffer_simd[i] = buffer_simd_mt[i] = (char)r;
    }

    printf("Buffer size: %d MB\n", N / (1024 * 1024));
    printf("Threads used: %d\n", THREADS);

    clock_t start, end;
    double mt_time, simd_time, simd_mt_time;

    start = clock();
    only_threads();
    end = clock();
    mt_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    only_simd();
    end = clock();
    simd_time = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    simd_plus_threads();
    end = clock();
    simd_mt_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nMultithreading time:      %.3f sec\n", mt_time);
    printf("SIMD time:                %.3f sec\n", simd_time);
    printf("SIMD + Multithreading:    %.3f sec\n", simd_mt_time);

    free(buffer_mt);
    free(buffer_simd);
    free(buffer_simd_mt);
}