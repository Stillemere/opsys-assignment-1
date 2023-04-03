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

pthread_mutex_t mutex;

void *consumer(void *arg) {
    cerr << "c: consumer invoked!\n";
    int item = 0;
    while (1) {
	    cerr << "c: waiting for full slot.\n";
        sem_wait(full); // wait for full slot
	    cerr << "c: entering critical section\n";
        pthread_mutex_lock(&mutex); // enter critical section
	    cerr << "c: getting item from table\n";
        item = table[i-1]; // get item from table
        printf("Consumer: Consumed item %d\n", item);
        i--;
        pthread_mutex_unlock(&mutex); // exit critical section
            cerr << "c: exited critical section.\n";
        sem_post(empty); // signal that table is empty
	    cerr << "c: table is empty.";
        if (i == 0) {
            printf("Table is empty. Consumer is waiting.\n");
        }
    }
    pthread_exit(NULL);
}

int main() {
    cerr << "c:(main) entered main.\n";
    empty = sem_open("empty", O_CREAT, 0644, TABLE_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    //sem_init(&sem, 0, TABLE_SIZE);

    pthread_mutex_init(&mutex, NULL);

    pthread_t cons_tid;
    pthread_create(&cons_tid, NULL, consumer, NULL);

    pthread_join(cons_tid, NULL);

    /*
    sem_close(&sem);
    sem_unlink("sem");
    */
    sem_close(empty);
    sem_close(full);
    sem_unlink("empty");
    sem_unlink("full");
    
    pthread_mutex_destroy(&mutex);
    cerr << "c:(main) end of main.\n";
    return 0;
}

