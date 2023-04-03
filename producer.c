#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
using std::cerr;

#define TABLE_SIZE 2

int table[TABLE_SIZE];
int i = 0;

sem_t *empty;
sem_t *full;
//sem_t sem;

pthread_mutex_t mutex;

void *producer(void *arg) {
    cerr << "p: Producer invoked!\n";
    int item = 0;
    while (1) {
        item = rand() % 100; // generate item
	cerr << "p: item generated. waiting for empty slot.\n";
        sem_wait(empty); // wait for empty slot
		cerr << "p: empty slot available.\n";
        pthread_mutex_lock(&mutex); // enter critical section
		cerr << "p: entering critical section.\n";
        table[i] = item; // put item on table
		cerr << "p: item placed on table.\n";
        printf("p: Producer: Produced item %d\n", item);
        i++;
        pthread_mutex_unlock(&mutex); // exit critical section
		cerr << "p: exiting critical section.\n";
        sem_post(full); // signal that table is full
		cerr << "p: table is full.\n";
        if (i == TABLE_SIZE) {
            printf("p: Table is full. Producer is waiting.\n");
        }
    }
    pthread_exit(NULL);
}

int main() {
    cerr << "\np:(main) producer.c now running\n";

    empty = sem_open("empty", O_CREAT, 0644, TABLE_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    //sem_init(&sem, 0, TABLE_SIZE);

    pthread_mutex_init(&mutex, NULL);

    pthread_t prod_tid;

    pthread_create(&prod_tid, NULL, producer, NULL); cerr << "p:(main) producer thread created.\n";


    pthread_join(prod_tid, NULL);   // wait for producer thread to exit
    cerr << "p:(main) producer thread exited";

    sem_close(empty); cerr << "p:(main) semiphore close empty";
    sem_close(full);	cerr << "p:(main) semiphore close full";
    sem_unlink("empty");	cerr << "p:(main) semiphore unlink empty";
    sem_unlink("full");		cerr << "p:(main) semiphore unlink full";

    pthread_mutex_destroy(&mutex);	cerr << "p:(main) mutex destroy";
    
    printf("p:(main) completed main.\n");

    return 0;
}

