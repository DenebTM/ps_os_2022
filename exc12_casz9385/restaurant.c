#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/random.h>
#include <time.h>

#include "common.h"
#include "myqueue.h"

pthread_mutex_t mutex;

myqueue order_queue;
order_num on_counter = -1;
order_num last_order = 0;

pthread_condattr_t ptcondattraaaaaa;

bool go_home = false;
bool enable_notifications = false;

void sleep_ms(int lower, int upper, unsigned int* rand_state) {
    if (upper > lower) {
        int maxdiff = upper - lower;
        int rand_scale = RAND_MAX / maxdiff;
        int rand_sleep = lower + (rand_r(rand_state) / rand_scale);

        usleep(rand_sleep * 1000);
    } else {
        usleep(lower * 1000);
    }
}

void prepare_order(int cook_num, unsigned int* rand_state) {
    pthread_mutex_lock(&mutex);

    if (myqueue_is_empty(&order_queue)) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    order_t* preparing = myqueue_pop(&order_queue);
    printf("Cook %d is preparing order %d\n", cook_num, preparing->num);
    pthread_mutex_unlock(&mutex);

    sleep_ms(100, 500, rand_state);
    printf("Cook %d has finished order %d\n", cook_num, preparing->num);

    pthread_mutex_lock(&mutex);
    while (on_counter != -1) {
        pthread_mutex_unlock(&mutex);
        sleep_ms(100, 0, NULL);
        pthread_mutex_lock(&mutex);
    } // wait until counter is empty
    on_counter = preparing->num;
    if (enable_notifications) {
        pthread_cond_signal(&(preparing->notification));
    }
    printf("Cook %d has placed order %d on counter\n", cook_num, preparing->num);

    pthread_mutex_unlock(&mutex);
}

void* cook_thread(void* arg) {
    int cook_num = (int)(uintptr_t)arg;

    struct timespec current;
    clock_gettime(CLOCK_MONOTONIC_RAW, &current);
    unsigned int rand_seed = (unsigned int)current.tv_nsec;

    while (!go_home) {
        prepare_order(cook_num, &rand_seed);
    }

    return NULL;
}

order_t* place_order(int guest_num) {
    pthread_mutex_lock(&mutex);

    order_t* my_order = malloc(sizeof(order_t));
    my_order->num = last_order++;
    if (enable_notifications) {
        pthread_cond_init(&(my_order->notification), &ptcondattraaaaaa);
    }
    myqueue_push(&order_queue, my_order);
    printf("Guest %d has made meal order %d\n", guest_num, my_order->num);
    
    pthread_mutex_unlock(&mutex);
    return my_order;
}

uint64_t try_pickup_order(int guest_num, struct timespec* start, order_t* my_order) {
    pthread_mutex_lock(&mutex);

    if (enable_notifications) {
        while ((on_counter != my_order->num)) {
            pthread_cond_wait(&my_order->notification, &mutex);
        }
    } else {
        sleep_ms(100, 0, NULL);
    }
    if (on_counter == my_order->num) {
        on_counter = -1;

        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        uint64_t ms_diff = (end.tv_sec * 1e3 + end.tv_nsec / 1e6)
                         - ((*start).tv_sec * 1e3 + (*start).tv_nsec / 1e6);
        printf("Guest %d has picked up order %d after %lu ms\n", guest_num, my_order->num, ms_diff);
        pthread_mutex_unlock(&mutex);
        return ms_diff;
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

void* guest_thread(void* arg) {
    int guest_num = (int)(uintptr_t)arg;

    order_t* my_order = place_order(guest_num);

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int ms_diff = 0;
    while ((ms_diff = try_pickup_order(guest_num, &start, my_order)) == -1) {
        sleep_ms(100, 0, NULL);
    }

    free(my_order);
    return (void*)(uintptr_t)ms_diff;
}

int main(int argc, char** argv)
{
    CATCH(argc < 4, "Usage: %s <enable notifications> <number of guests> <number of cooks>\n", argv[0]);

    enable_notifications = (atoi(argv[1]) > 0);
    int guests_count = atoi(argv[2]);
    int cooks_count = atoi(argv[3]);

    pthread_attr_t ptattr;
    pthread_attr_init(&ptattr);

    pthread_t guests[guests_count];
    pthread_t cooks[cooks_count];

    if (enable_notifications) {
        pthread_condattr_init(&ptcondattraaaaaa);
    }
    myqueue_init(&order_queue);

    for (int i = 0; i < guests_count; i++) {
        pthread_create(&guests[i], &ptattr, &guest_thread, (void*)(uintptr_t)i);
    }
    for (int i = 0; i < cooks_count; i++) {
        pthread_create(&cooks[i], &ptattr, &cook_thread, (void*)(uintptr_t)i);
    }


    uint64_t guest_ms[guests_count];
    for (int i = 0; i < guests_count; i++) {
        pthread_join(guests[i], (void*)&guest_ms[i]);
    }

    uint64_t ms_total = 0;
    for (int i = 0; i < guests_count; i++) {
        ms_total += guest_ms[i];
    }
    ms_total /= guests_count;
    printf("All guests have been served with an average wait time of %lu ms\n", ms_total);
    go_home = true;
    for (int i = 0; i < cooks_count; i++) {
        pthread_join(cooks[i], NULL);
    }
    
    return EXIT_SUCCESS;
}
