#ifndef COMMON_H_
#define COMMON_H_

typedef int order_num;
typedef struct order {
    order_num num;
    pthread_cond_t notification;
} order_t;

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

#endif