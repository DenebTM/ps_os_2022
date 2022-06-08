#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdatomic.h>
#include <sched.h>
#include "myqueue.h"

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

#define NUM_ONES 1000000

#ifdef USE_MY_MUTEX
    #define mutex_t my_mutex_t
    #define mutex_lock my_mutex_lock
    #define mutex_unlock my_mutex_unlock
#else
    #define mutex_t pthread_mutex_t
    #define mutex_lock pthread_mutex_lock
    #define mutex_unlock pthread_mutex_unlock
#endif


typedef atomic_flag my_mutex_t;
void my_mutex_lock(my_mutex_t* mutex) {
    while (atomic_flag_test_and_set(mutex)) {
        sched_yield();
    }
}
void my_mutex_unlock(my_mutex_t* mutex) {
    assert(atomic_flag_test_and_set(mutex) == true);
    atomic_flag_clear(mutex);
}

mutex_t mutex;
myqueue queue;

void* producer(void* arg) {
    for (int i = 0; i < NUM_ONES + 1; i++) {
        mutex_lock(&mutex);
        int val = i < NUM_ONES ? 1 : 0;
        myqueue_push(&queue, val);
        mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    int sum = 0;
    bool done = false;
    while (!done) {
        mutex_lock(&mutex);
        if (!myqueue_is_empty(&queue)) {
            int val = myqueue_pop(&queue);
            sum += val;
            done = (val == 0);
        }
        mutex_unlock(&mutex);
    }
    printf("%d\n", sum);

    return NULL;
}

int main(int argc, char const *argv[]) {
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);

    myqueue_init(&queue);

    pthread_t pt_producer, pt_consumer;
    pthread_create(&pt_producer, &pt_attr, &producer, NULL);
    pthread_create(&pt_consumer, &pt_attr, &consumer, NULL);

    pthread_join(pt_producer, NULL);
    pthread_join(pt_consumer, NULL);

    return EXIT_SUCCESS;
}
