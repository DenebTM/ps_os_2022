#ifndef _MY_MALLOC_H
#define _MY_MALLOC_H

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define __BLOCK_MIN_SIZE__ sizeof(void*)

#define __BLOCK_HEADER_SIZE__ sizeof(struct __block__)
struct __block__ {
    struct __block__* next_free;
    size_t size;
    char mem[];
};

extern struct __free_list__ {
    pthread_mutex_t mutex;
    size_t size;
    struct __block__* head;
    struct __block__* first_free;
} __my_pool__;

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);

#endif
