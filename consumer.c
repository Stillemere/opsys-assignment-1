
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <iostream>
using std::cout; using std::cerr; using std::endl;

#define TABLE_SIZE 2
sem_t mutex;
int i = 0;
typedef struct{
    int items[TABLE_SIZE];
} Table;
Table *table;

void *consumer(void *arg) {
    cerr << "c: consumer invoked!\n";

    int item = 0;
    while (1) {
	cout << "c: waiting for full slot." << endl;
        sem_wait(&mutex); // wait
	cout << "c: entering critical section" << endl;
        // Critical section
	{
        //pthread_mutex_lock(&mutex); // enter critical section


	cout << "c: getting item from table" << endl;
        item = table->items[i-1]; // get item from table
        cout << "Consumer: Consumed item" << item << endl;
        i--;
        //pthread_mutex_unlock(&mutex); // exit critical section
        cout << "c: exited critical section." << endl;


        } // end critical section
        sem_post(&mutex); // exiting critical secion
        if (i == 0) {
            cout << "Table is empty. Consumer is waiting." << endl;
        }
    }
    pthread_exit(NULL); // exit thread
}

int main() {
    cerr << "c:(main) entered main." << endl;

    //empty = sem_open("empty", O_CREAT, 0644, TABLE_SIZE);
    //full = sem_open("full", O_CREAT, 0644, 0);
    pthread_t consumer_thread;


    // create shared memory object
    // CHANGE: producer.c should do this
    int fd = shm_open(NULL, O_RDWR, 0); // Reading existing shared memory obj
    //if(fd == -1) errExit("shm_open");
    sem_init(&mutex, 1, 1); // initialize semaphore

    // create map structure
    table = (Table *)mmap(NULL, (sizeof(int)*2), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //	if(map == MAP_FAILED) errExit("mmap");
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    pthread_join(consumer_thread, NULL); // enters consumer thread

    std::cout << "c: (main) consumer thread exited)" << std::endl;

    // copy data into shared memory object
    //memcpy(&table->items, table, TABLE_SIZE);
    // tell 

    /*
    pthread_mutex_init(&mutex, NULL);

    pthread_t cons_tid;
    pthread_create(&cons_tid, NULL, consumer, NULL);

    pthread_join(cons_tid, NULL);
    */

    sem_close(&mutex); // close semaphore
    sem_unlink("mutex"); // seems right?

    //shm_unlink(&fd); // remove shared memory object

    //pthread_mutex_destroy(&mutex);
    cerr << "c:(main) end of main.\n";
    return 0;
}

