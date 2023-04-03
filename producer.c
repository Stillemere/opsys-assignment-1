#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TABLE_SIZE 2

int table[TABLE_SIZE];
int index = 0;

sem_t *empty;
sem_t *full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int item = 0;
    while (1) {
        item = rand() % 100; // generate item
        sem_wait(empty); // wait for empty slot
        pthread_mutex_lock(&mutex); // enter critical section
        table[index] = item; // put item on table
        printf("Producer: Produced item %d\n", item);
        index++;
        pthread_mutex_unlock(&mutex); // exit critical section
        sem_post(full); // signal that table is full
        if (index == TABLE_SIZE) {
            printf("Table is full. Producer is waiting.\n");
        }
    }
    pthread_exit(NULL);
}

int main() {
    empty = sem_open("empty", O_CREAT, 0644, TABLE_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t prod_tid;
    pthread_create(&prod_tid, NULL, producer, NULL);

    pthread_join(prod_tid, NULL);

    sem_close(empty);
    sem_close(full);
    sem_unlink("empty");
    sem_unlink("full");

    pthread_mutex_destroy(&mutex);

    return 0;
}

