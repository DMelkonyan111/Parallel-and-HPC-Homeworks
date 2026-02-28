#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define P 4
#define R 5
#define N 5
#define S 4
#define M 5

pthread_barrier_t barrier1;
pthread_mutex_t mutex1;
int rolls[P];
int wins[P];

void* dice_player(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < R; i++) {
        rolls[id] = rand() % 6 + 1;
        pthread_barrier_wait(&barrier1);

        if (id == 0) {
            int max = 0;
            int winner = 0;

            for (int j = 0; j < P; j++) {
                if (rolls[j] > max) {
                    max = rolls[j];
                    winner = j;
                }
            }

            wins[winner]++;
            printf("Round %d winner: Player %d\n", i + 1, winner);
        }

        pthread_barrier_wait(&barrier1);
    }
    return NULL;
}

void assignment1() {
    pthread_t threads[P];
    int ids[P];
    pthread_barrier_init(&barrier1, NULL, P);
    pthread_mutex_init(&mutex1, NULL);

    for (int i = 0; i < P; i++) {
        wins[i] = 0;
        ids[i] = i;
        pthread_create(&threads[i], NULL, dice_player, &ids[i]);
    }

    for (int i = 0; i < P; i++) {
        pthread_join(threads[i], NULL);
    }

    int max = 0, winner = 0;

    for (int i = 0; i < P; i++) {
        if (wins[i] > max) {
            max = wins[i];
            winner = i;
        }
    }

    printf("Overall winner: Player %d\n\n", winner);
    pthread_barrier_destroy(&barrier1);
    pthread_mutex_destroy(&mutex1);
}

pthread_barrier_t barrier2;

void* lobby_player(void* arg) {
    int id = *(int*)arg;
    sleep(rand() % 3);

    printf("Player %d ready\n", id);
    pthread_barrier_wait(&barrier2);
    printf("Game Started! Player %d\n", id);

    return NULL;
}

void assignment2() {
    int ids[N];
    pthread_t threads[N];
    pthread_barrier_init(&barrier2, NULL, N);

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, lobby_player, &ids[i]);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier2);
    printf("\n");
}

pthread_barrier_t barrier3;
double temps[S];

void* sensor(void* arg) {
    int id = *(int*)arg;
    temps[id] = (rand() % 3000) / 100.0;
    pthread_barrier_wait(&barrier3);
    return NULL;
}

void assignment3() {
    int ids[S];
    pthread_t threads[S];
    pthread_barrier_init(&barrier3, NULL, S);

    for (int i = 0; i < S; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, sensor, &ids[i]);
    }

    for(int i = 0; i < S; i++) {
        pthread_join(threads[i], NULL);
    }

    double sum = 0;
    for(int i = 0; i < S; i++) {
        sum += temps[i];
    }

    printf("Average temperature: %.2f\n\n", sum / S);
    pthread_barrier_destroy(&barrier3);
}

pthread_barrier_t b1, b2, b3;

void* pipeline_worker(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d Stage 1\n", id);
    pthread_barrier_wait(&b1);
    printf("Thread %d Stage 2\n", id);
    pthread_barrier_wait(&b2);
    printf("Thread %d Stage 3\n", id);
    pthread_barrier_wait(&b3);

    return NULL;
}

void assignment4() {
    int ids[M];

    pthread_t threads[M];
    pthread_barrier_init(&b1, NULL, M);
    pthread_barrier_init(&b2, NULL, M);
    pthread_barrier_init(&b3, NULL, M);

    for (int i = 0; i < M; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, pipeline_worker, &ids[i]);
    }

    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&b1);
    pthread_barrier_destroy(&b2);
    pthread_barrier_destroy(&b3);
}

int main() {
    srand(time(NULL));     // In order to guarantee a random number every time

    // assignment1();
    // assignment2();
    // assignment3();
    assignment4();
    return 0;
}