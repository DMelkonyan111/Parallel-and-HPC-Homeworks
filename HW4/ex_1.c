#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <immintrin.h>

#define N (100 * 1024 * 1024)
long count[4] = {0, 0, 0, 0};

typedef struct
{
    char *dna_string;
    long *dna_count;
    int start;
    int end;
} thread_args;

pthread_mutex_t lock;
char *dna_sequence;

void *countingFunction(void *args)
{
    thread_args *data = (thread_args *)args;
    long local_count[] = {0, 0, 0, 0};

    for (int i = data->start; i < data->end; i++)
    {
        if (data->dna_string[i] == 'A')
        {
            local_count[0]++;
        }
        else if (data->dna_string[i] == 'C')
        {
            local_count[1]++;
        }
        else if (data->dna_string[i] == 'G')
        {
            local_count[2]++;
        }
        else
        {
            local_count[3]++;
        }
    }

    pthread_mutex_lock(&lock);

    data->dna_count[0] += local_count[0];
    data->dna_count[1] += local_count[1];
    data->dna_count[2] += local_count[2];
    data->dna_count[3] += local_count[3];

    pthread_mutex_unlock(&lock);
}

void only_threads()
{
    pthread_mutex_init(&lock, NULL);

    pthread_t t1, t2, t3, t4;

    int chunk = N / 4;

    thread_args args1 = {dna_sequence, count, 0, chunk};
    thread_args args2 = {dna_sequence, count, chunk, chunk * 2};
    thread_args args3 = {dna_sequence, count, chunk * 2, chunk * 3};
    thread_args args4 = {dna_sequence, count, chunk * 3, chunk * 4};

    pthread_create(&t1, NULL, countingFunction, &args1);
    pthread_create(&t2, NULL, countingFunction, &args2);
    pthread_create(&t3, NULL, countingFunction, &args3);
    pthread_create(&t4, NULL, countingFunction, &args4);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);


    pthread_mutex_destroy(&lock);
}

void only_simd()
{
    __m256i vec_A = _mm256_set1_epi8('A');
    __m256i vec_C = _mm256_set1_epi8('C');
    __m256i vec_G = _mm256_set1_epi8('G');
    __m256i vec_T = _mm256_set1_epi8('T');

    int i = 0;
    for (; i <= N - 32; i += 32)
    {
        __m256i vec = _mm256_loadu_si256((__m256i *)(dna_sequence + i));

        __m256i maskA = _mm256_cmpeq_epi8(vec, vec_A);
        __m256i maskC = _mm256_cmpeq_epi8(vec, vec_C);
        __m256i maskG = _mm256_cmpeq_epi8(vec, vec_G);
        __m256i maskT = _mm256_cmpeq_epi8(vec, vec_T);

        count[0] += _mm_popcnt_u32(_mm256_movemask_epi8(maskA));
        count[1] += _mm_popcnt_u32(_mm256_movemask_epi8(maskC));
        count[2] += _mm_popcnt_u32(_mm256_movemask_epi8(maskG));
        count[3] += _mm_popcnt_u32(_mm256_movemask_epi8(maskT));
    }

    for (; i < N; i++)
    {
        char c = dna_sequence[i];

        if (c == 'A')
        {
            count[0]++;
        }
        else if (c == 'C')
        {
            count[1]++;
        }
        else if (c == 'G')
        {
            count[2]++;
        }
        else
        {
            count[3]++;
        }
    }
}

void *simd_thread(void *arg)
{
    thread_args *data = (thread_args *)arg;
    long local_count[4] = {0, 0, 0, 0};

    __m256i vec_A = _mm256_set1_epi8('A');
    __m256i vec_C = _mm256_set1_epi8('C');
    __m256i vec_G = _mm256_set1_epi8('G');
    __m256i vec_T = _mm256_set1_epi8('T');

    int i = data->start;
    for (; i <= data->end - 32; i += 32)
    {
        __m256i vec = _mm256_loadu_si256((__m256i *)(data->dna_string + i));

        __m256i maskA = _mm256_cmpeq_epi8(vec, vec_A);
        __m256i maskC = _mm256_cmpeq_epi8(vec, vec_C);
        __m256i maskG = _mm256_cmpeq_epi8(vec, vec_G);
        __m256i maskT = _mm256_cmpeq_epi8(vec, vec_T);

        local_count[0] += _mm_popcnt_u32(_mm256_movemask_epi8(maskA));
        local_count[1] += _mm_popcnt_u32(_mm256_movemask_epi8(maskC));
        local_count[2] += _mm_popcnt_u32(_mm256_movemask_epi8(maskG));
        local_count[3] += _mm_popcnt_u32(_mm256_movemask_epi8(maskT));
    }

    for (; i < data->end; i++)
    {
        char c = data->dna_string[i];

        if (c == 'A')
        {
            local_count[0]++;
        }
        else if (c == 'C')
        {
            local_count[1]++;
        }
        else if (c == 'G')
        {
            local_count[2]++;
        }
        else
        {
            local_count[3]++;
        }
    }

    pthread_mutex_lock(&lock);
    data->dna_count[0] += local_count[0];
    data->dna_count[1] += local_count[1];
    data->dna_count[2] += local_count[2];
    data->dna_count[3] += local_count[3];
    pthread_mutex_unlock(&lock);

    return NULL;
}

void simd_plus_threads()
{
    pthread_mutex_init(&lock, NULL);

    pthread_t t1, t2, t3, t4;
    int chunk = N / 4;

    thread_args args1 = {dna_sequence, count, 0, chunk};
    thread_args args2 = {dna_sequence, count, chunk, chunk * 2};
    thread_args args3 = {dna_sequence, count, chunk * 2, chunk * 3};
    thread_args args4 = {dna_sequence, count, chunk * 3, N};

    pthread_create(&t1, NULL, simd_thread, &args1);
    pthread_create(&t2, NULL, simd_thread, &args2);
    pthread_create(&t3, NULL, simd_thread, &args3);
    pthread_create(&t4, NULL, simd_thread, &args4);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    pthread_mutex_destroy(&lock);
}

#include <stdio.h>

void scalar()
{
    long scalar_count[4] = {0, 0, 0, 0}; // A, C, G, T

    for (int i = 0; i < N; i++)
    {
        char c = dna_sequence[i];

        if (c == 'A')
        {
            scalar_count[0]++;
        }
        else if (c == 'C')
        {
            scalar_count[1]++;
        }
        else if (c == 'G')
        {
            scalar_count[2]++;
        }
        else
        {
            scalar_count[3]++;
        }
    }
}

void main()
{
    srand(time(NULL));

    // The generation of a random DNA sequence
    dna_sequence = malloc((N + 1) * sizeof(char));

    for (int i = 0; i < N; i++)
    {
        int random = rand() % 4;

        if (random == 0)
        {
            dna_sequence[i] = 'A';
        }
        else if (random == 1)
        {
            dna_sequence[i] = 'C';
        }
        else if (random == 2)
        {
            dna_sequence[i] = 'G';
        }
        else
        {
            dna_sequence[i] = 'T';
        }
    }

    printf("DNA size: %d MB\n", N / (1024 * 1024));
    printf("Threads used: 4\n");

    clock_t start, end;
    double scalar_time, mt_time, simd_time, simd_mt_time;

    start = clock();
    scalar();
    end = clock();
    scalar_time = (double)(end - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < 4; i++) count[i] = 0; // reset counts

    start = clock();
    only_threads(); 
    end = clock();
    mt_time = (double)(end - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < 4; i++) count[i] = 0; 

    start = clock();
    only_simd(); 
    end = clock();
    simd_time = (double)(end - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < 4; i++) count[i] = 0;

    start = clock();
    simd_plus_threads();
    end = clock();
    simd_mt_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nCounts (A C G T):\n%ld %ld %ld %ld\n\n", count[0], count[1], count[2], count[3]);

    printf("Scalar time:                %.3f sec\n", scalar_time);
    printf("Multithreading time:        %.3f sec\n", mt_time);
    printf("SIMD time:                  %.3f sec\n", simd_time);
    printf("SIMD + Multithreading time: %.3f sec\n", simd_mt_time);

    free(dna_sequence);

}