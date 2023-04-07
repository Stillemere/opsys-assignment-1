
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

	cout << "c: getting item from table" << endl;
        item = table->items[i]; // get item from table, removed minus 1
        cout << "Consumer: Consumed item" << item << endl;
        i--;
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


    pthread_t consumer_thread;

    sem_open("mutex", O_RDWR, 0666, TABLE_SIZE);
    std::cout << "c: opened mutex" << std::endl;
    // create shared memory object
    int fd = shm_open(NULL, O_RDWR, 0); // Reading existing shared memory obj
    std::cout << "c: opened shared memory" << std::endl;
    
    // initialize semaphore
    sem_init(&mutex, 1, 1);
    std::cout << "c: initialized semaphore" << std::endl;

    // map the table
    table = (Table *)mmap(NULL, (sizeof(int)*2), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    std::cout << "c:mapped table" << std::endl;

    // create thread
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    pthread_join(consumer_thread, NULL); // enters consumer thread

    std::cout << "c: (main) consumer thread exited)" << std::endl;

    //memcpy(&table->items, table, TABLE_SIZE);

    sem_close(&mutex); // close semaphore
    sem_unlink("mutex"); // seems right?

    //shm_unlink(&fd); // remove shared memory object

    cerr << "c:(main) end of main.\n";
    return 0;
}

