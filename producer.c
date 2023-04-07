#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/mman.h>


#include <iostream>

#define TABLE_SIZE 2
int i = 0;
sem_t mutex;

typedef struct {
    int items[TABLE_SIZE];
} Table;
Table *table;


void *producer(void *arg) {
    std::cout << "producer.c: Producer invoked!" << std::endl;
    int item = 0;
    while (1) {
        item = rand() % 100; // generate item
	std::cout << "p: item generated. waiting for empty slot." << std::endl;
        sem_wait(&mutex); // wait for empty slot
		std::cout << "p: empty slot available." << std::endl;
        //pthread_mutex_lock(&mutex); // enter critical section
		std::cout << "p: entering critical section." << std::endl;
        // Critical Section
	{
            table->items[i] = item; // put item on table
            std::cout << "p: Producer: Produced item" << item << std::endl;
            i++;
           // pthread_mutex_unlock(&mutex); // exit critical section
		std::cout << "p: exiting critical section." << std::endl;
            sem_post(&mutex);
            if (i == TABLE_SIZE) {
                std::cout << "p: Table is full. Producer is waiting." << std::endl;
            }
        }
    }
    pthread_exit(NULL);
}

int main(){
    std::cout << "producer.c: running main" << std::endl;

    pthread_t producer_thread;
    int fd = shm_open("shmname", O_CREAT | O_RDWR, 0666); // creating shared memory obj
//  ftruncate(fd, sizeof(int)*2);
    table = (Table *)mmap(NULL, (sizeof(int)*2), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //if(shmp == MAP_FAILED) std::cout << "mmap failed." << std::endl;

    sem_init(&mutex, 1, 1); // pthread_mutex_init(&mutex, NULL);
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_join(producer_thread, NULL); // wait for producer thread to exit
    // jumps to producer thread
    std::cout << "p:(main) producer thread exited" << std::endl;

    sem_close(&mutex);
    sem_unlink("mutex");
    //pthread_mutex_destroy(&mutex);

    std::cout << "p:(main) completed main." << std::endl;

    return 0;
}

