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

struct _job {
    job_function fun;
    job_arg arg;
    bool finished;
};
typedef struct _job* job_id;

struct _jq_entry {
    struct _job *job;
    STAILQ_ENTRY(_jq_entry) entries;
};

STAILQ_HEAD(_jq, _jq_entry);

struct _worker_attr {
    pthread_mutex_t* mutex;
    pthread_cond_t* ptcond;
    struct _jq* job_queue;
    bool* pool_ready;
    bool* exit;
};

typedef struct thread_pool_data {
    pthread_mutex_t mutex;
    pthread_cond_t ptcond;
    size_t workers_count;
    bool _ready;
    bool _exit;
    struct _jq job_queue;
    pthread_t* workers;
} thread_pool;

static void* _worker_idle(void* arg) {
    struct _worker_attr* wattr = (struct _worker_attr*)arg;
    pthread_cond_t* ptcond = wattr->ptcond;
    pthread_mutex_t* mutex = wattr->mutex;
    struct _jq* job_queue = wattr->job_queue;
    bool* pool_ready = wattr->pool_ready;
    bool* pool_exit = wattr->exit;

    while (!*pool_ready) {
        usleep(30000);
    }
    
    while (!*pool_exit) {
        pthread_mutex_lock(mutex);
        while (!*pool_exit && STAILQ_EMPTY(job_queue)) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_nsec += 1000;

            pthread_cond_timedwait(ptcond, mutex, &ts);
        }

        if (!*pool_exit && !STAILQ_EMPTY(job_queue)) {
            struct _jq_entry* entry = STAILQ_FIRST(job_queue);
            struct _job* job;
            job = entry->job;
            STAILQ_REMOVE_HEAD(job_queue, entries);
            free(entry);

            job->fun(job->arg);
            job->finished = true;
        }

        pthread_mutex_unlock(mutex);
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
    
    pool->workers_count = size;
    pool->_exit = false;
    pool->_ready = false;

    STAILQ_INIT(&pool->job_queue);

    pool->workers = (pthread_t*)malloc(pool->workers_count * sizeof(pthread_t));

    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);

    struct _worker_attr wattr;
    wattr.mutex = &pool->mutex;
    wattr.ptcond = &pool->ptcond;
    wattr.job_queue = &pool->job_queue;
    wattr.exit = &pool->_exit;
    wattr.pool_ready = &pool->_ready;
    for (size_t i = 0; i < size; i++) {
        pthread_create(&pool->workers[i], &pt_attr, &_worker_idle, &wattr);
    }

    pool->_ready = true;
}

static job_id pool_submit(thread_pool* pool, job_function start_routine, job_arg arg) {
    struct _jq_entry* entry = (struct _jq_entry*)malloc(sizeof(struct _jq_entry));
    struct _job* job = (struct _job*)malloc(sizeof(struct _job));

    job->fun = start_routine;
    job->arg = arg;
    job->finished = false;
    entry->job = job;

    pthread_mutex_lock(&pool->mutex);
    STAILQ_INSERT_TAIL(&pool->job_queue, entry, entries);
    pthread_cond_signal(&pool->ptcond);
    pthread_mutex_unlock(&pool->mutex);

    return job;
}

static void pool_await(job_id id) {
    while (!id->finished) {
        sched_yield();
    }

    free(id);
}

static void pool_destroy(thread_pool* pool) {
    pool->_exit = true;
    for (size_t i = 0; i < pool->workers_count; i++) {
        pthread_join(pool->workers[i], NULL);
    }

    free(pool->workers);
}

#endif
