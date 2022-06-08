#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdatomic.h>

#define INIT_VAL  100000
#define NUM_THREADS 1000
#define NUM_OPS    10000

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

#ifdef USE_ATOMICS
    atomic_int_fast64_t X;
#else
    int64_t X = INIT_VAL;
    pthread_mutex_t mutex;
#endif

void* decrement(void* arg) {
    for (int i = 0; i < NUM_OPS; i++) {
        #ifdef USE_ATOMICS
            atomic_fetch_sub(&X, 1);
        #else
            pthread_mutex_lock(&mutex);
            --X;
            pthread_mutex_unlock(&mutex);
        #endif
    }
    return NULL;
}

int main()
{
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);

    #ifdef USE_ATOMICS
        atomic_init(&X, INIT_VAL);
    #else
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutex_init(&mutex, &mutex_attr);
    #endif

    pthread_t pts[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&pts[i], &pt_attr, &decrement, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(pts[i], NULL);
    }

    #ifdef USE_ATOMICS
        printf("%ld\n", atomic_load(&X));
    #else
        printf("%ld\n", X);
    #endif

    return EXIT_SUCCESS;
}
