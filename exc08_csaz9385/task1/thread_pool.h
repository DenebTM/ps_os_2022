#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <assert.h>
#include <sys/queue.h>

typedef void* (*job_function)(void*);
typedef void* job_arg;
typedef size_t job_id;

struct _job {
    job_id id;
    job_function fun;
    job_arg arg;
    bool finished;
};

struct _jq_entry {
    struct _job *job;
    STAILQ_ENTRY(_jq_entry) entries;
};

STAILQ_HEAD(_jq, _jq_entry);

#define MAX_JOBS 65536
typedef struct thread_pool_data {
    pthread_mutex_t mutex;
    pthread_cond_t ptcond;
    job_id last_id;
    bool _ready;
    bool _exit;
    size_t workers_count;
    pthread_t* workers;
    struct _jq job_queue;
    struct _job** jobs;
} thread_pool;

static void* _worker_idle(void* arg) {
    thread_pool* pool = (thread_pool*)arg;
    pthread_cond_t* ptcond = &pool->ptcond;
    pthread_mutex_t* mutex = &pool->mutex;
    struct _jq* job_queue = &pool->job_queue;
    bool* pool_ready = &pool->_ready;
    bool* pool_exit = &pool->_exit;

    // ugly, but speeds up initialization of large pools substantially
    while (!*pool_ready) {
        usleep(50000);
    }
    
    while (!*pool_exit) {
        pthread_mutex_lock(mutex);
        while (!*pool_exit && STAILQ_EMPTY(job_queue)) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;

            pthread_cond_timedwait(ptcond, mutex, &ts);
        }

        if (!*pool_exit) {
            struct _jq_entry* entry = STAILQ_FIRST(job_queue);
            struct _job* job;
            job = entry->job;
            STAILQ_REMOVE_HEAD(job_queue, entries);
            free(entry);

            job->fun(job->arg);
            job->finished = true;
        }

        pthread_mutex_unlock(mutex);
        sched_yield();
    }

    return NULL;
}


static void pool_create(thread_pool* pool, size_t size) {
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&pool->mutex, &mutex_attr);

    pthread_condattr_t ptcond_attr;
    pthread_condattr_init(&ptcond_attr);
    pthread_cond_init(&pool->ptcond, &ptcond_attr);
    
    pool->_exit = false;
    pool->_ready = false;

    STAILQ_INIT(&pool->job_queue);

    pool->workers_count = size;
    pool->workers = (pthread_t*)malloc(pool->workers_count * sizeof(pthread_t));
    
    pool->last_id = 0;
    pool->jobs = (struct _job**)calloc(MAX_JOBS, sizeof(struct _job*));

    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    for (size_t i = 0; i < size; i++) {
        pthread_create(&pool->workers[i], &pt_attr, &_worker_idle, pool);
    }

    pool->_ready = true;
}

static job_id pool_submit(thread_pool* pool, job_function start_routine, job_arg arg) {
    // try to find a free slot in the jobs array
    // if none is found, return -1 (error)
    int counter = 0;
    while (counter < MAX_JOBS && pool->jobs[pool->last_id]) {
        pool->last_id = (pool->last_id + 1) % MAX_JOBS;
        counter++;
    }
    if (counter == MAX_JOBS) {
        return -1;
    }

    struct _jq_entry* entry = (struct _jq_entry*)malloc(sizeof(struct _jq_entry));
    struct _job* job = (struct _job*)malloc(sizeof(struct _job));
    
    job->id = pool->last_id;
    job->fun = start_routine;
    job->arg = arg;
    job->finished = false;
    entry->job = job;
    pool->jobs[job->id] = job;

    pthread_mutex_lock(&pool->mutex);
    STAILQ_INSERT_TAIL(&pool->job_queue, entry, entries);
    pthread_cond_signal(&pool->ptcond);
    pthread_mutex_unlock(&pool->mutex);

    return job->id;
}

static void pool_await(thread_pool* pool, job_id id) {
    while (!pool->jobs[id]->finished) {
        sched_yield();
    }

    free(pool->jobs[id]);
    pool->jobs[id] = NULL;
}

static void pool_destroy(thread_pool* pool) {
    pool->_exit = true;
    for (size_t i = 0; i < pool->workers_count; i++) {
        pthread_join(pool->workers[i], NULL);
    }
    for (job_id i = 0; i < MAX_JOBS; i++) {
        if (pool->jobs[i]) {
            free(pool->jobs[i]);
        }
    }
    free(pool->workers);
    free(pool->jobs);
}

#endif
