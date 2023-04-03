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

void *consumer(void *arg) {
    int item = 0;
    while (1) {
        sem_wait(full); // wait for full slot
        pthread_mutex_lock(&mutex); // enter critical section
        item = table[index-1]; // get item from table
        printf("Consumer: Consumed item %d\n", item);
        index--;
        pthread_mutex_unlock(&mutex); // exit critical section
        sem_post(empty); // signal that table is empty
        if (index == 0) {
            printf("Table is empty. Consumer is waiting.\n");
        }
    }
    pthread_exit(NULL);
}

int main() {
    empty = sem_open("empty", O_CREAT, 0644, TABLE_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t cons_tid;
    pthread_create(&cons_tid, NULL, consumer, NULL);

    pthread_join(cons_tid, NULL);

    sem_close(empty);
    sem_close(full);
    sem_unlink("empty");
    sem_unlink("full");

    pthread_mutex_destroy(&mutex);

    return 0;
}

