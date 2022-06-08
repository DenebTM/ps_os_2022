#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>
#include <pthread.h>

#ifdef USE_THREADPOOL
#include "thread_pool.h"
#define WORKER_COUNT 128
#endif

#define JOB_COUNT 30000
#define INIT_VAL JOB_COUNT

void* worker(void* arg) {
    atomic_int* val = (atomic_int*)arg;

    atomic_fetch_sub(val, 1);

    return NULL;
}

int main() {
    atomic_int counter;
    atomic_init(&counter, INIT_VAL);

    #ifdef USE_THREADPOOL
        thread_pool pool;
        pool_create(&pool, WORKER_COUNT);

        job_id jobs[JOB_COUNT];
        for(int i = 0; i < JOB_COUNT; i++) {
            jobs[i] = pool_submit(&pool, &worker, &counter);
        }

        for (int i = 0; i < JOB_COUNT; i++) {
            pool_await(jobs[i]);
        }
        pool_destroy(&pool);
    #else
        pthread_attr_t pt_attr;
        pthread_attr_init(&pt_attr);

        pthread_t pts[JOB_COUNT];
        for (int i = 0; i < JOB_COUNT; i++) {
            pthread_create(&pts[i], &pt_attr, &worker, &counter);
        }
        
        for (int i = 0; i < JOB_COUNT; i++) {
            pthread_join(pts[i], NULL);
        }
    #endif

    printf("Result: %d\n", atomic_load(&counter));

    return EXIT_SUCCESS;
}
