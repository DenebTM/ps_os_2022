#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include "myqueue.h"

#define NUM_QUEUE_ELEMS 10000
#define NUM_THREADS 5

#define CATCH(exit_cond, go_to, ...)                \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit_status = EXIT_FAILURE;             \
            goto go_to;                             \
        }                                           \
    } while(0)

myqueue queue;
pthread_mutex_t mutex;

void* conSUMer(void* arg) {
    int consumer_num = (int)arg;
    int sum = 0;
    bool done = false;

    while (1) {
        pthread_mutex_lock(&mutex);
        if (!myqueue_is_empty(&queue)) {
            int num = myqueue_pop(&queue);
            if (num == 0) {
                done = true;
            }
            sum += num;
        }
        pthread_mutex_unlock(&mutex);
        if (done) {
            printf("Consumer %d sum: %d\n", consumer_num, sum);
            return (void*)sum;
        }
    }
}

int main()
{
    int exit_status = EXIT_SUCCESS;

    // boilerplate initialization code
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutex_init(&mutex, &mutexattr);
    myqueue_init(&queue);

    // start threads
    pthread_t pts[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        CATCH(pthread_create(&pts[i], &pt_attr, &conSUMer, (void*)i) != 0,
            cleanup,
            "Error on pthread_create: %s\n", strerror(errno));
    }

    // produce
    for (int i = 0; i < NUM_QUEUE_ELEMS + NUM_THREADS; i++) {
        pthread_mutex_lock(&mutex);

        if (i < NUM_QUEUE_ELEMS) {
            myqueue_push(&queue, 1);
        } else {
            myqueue_push(&queue, 0);
        }

        pthread_mutex_unlock(&mutex);
    }

    // wait for threads to finish and compute the final result
    int final_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        int thread_result;
        CATCH(pthread_join(pts[i], (void*)&thread_result) != 0,
            cleanup,
            "Error on pthread_join: %s\n", strerror(errno));
        
        final_sum += thread_result;
    }
    printf("Final sum: %d\n", final_sum);

    cleanup:
    pthread_mutex_destroy(&mutex);
    return exit_status;
}
