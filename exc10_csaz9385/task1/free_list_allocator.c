#include "free_list_allocator.h"

struct __free_list__ __my_pool__;

void my_allocator_destroy(void) {
    if (__my_pool__.head) {
        pthread_mutex_lock(&__my_pool__.mutex);
        munmap(__my_pool__.head, __my_pool__.size);
        __my_pool__.head = NULL;
        __my_pool__.next_free = NULL;
        pthread_mutex_unlock(&__my_pool__.mutex);
    }
}

void my_allocator_init(size_t size) {
    // attempt to destroy the pool in case it was initialized before
    my_allocator_destroy();

    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutex_init(&__my_pool__.mutex, &mutexattr);

    pthread_mutex_lock(&__my_pool__.mutex);

    // allocate space for the linked list and note down the size of the pool
    __my_pool__.head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    __my_pool__.next_free = __my_pool__.head;
    __my_pool__.size = size;
    size /= sizeof(struct __block__);

    // assign pointers of every block
    for (size_t i = 0; i < size; i++) {
        if (i < size-1) __my_pool__.head[i].next_free = &__my_pool__.head[i+1];
    }

    pthread_mutex_unlock(&__my_pool__.mutex);
}

void* my_malloc(size_t size) {
    if (size > __BLOCK_SIZE__ || !__my_pool__.head) {
        return NULL;
    }

    pthread_mutex_lock(&__my_pool__.mutex);

    struct __block__* this_block;
    void* mapaddr = NULL;
    if (!(this_block = __my_pool__.next_free)) {
        goto my_malloc_return;
    }
    
    mapaddr = &this_block->mem;

    // update next_free pointers of preceding blocks currently pointing to this one
    /* ... this is slow as hell actually, don't do this
    struct __block__* prev = this_block - 1;
    while (prev >= __my_pool__.head) {
        if (prev->next_free < this_block) break;
        if (prev->next_free == this_block) {
            prev->next_free = this_block->next_free;
        }
        prev--;
    } */
    __my_pool__.next_free = this_block->next_free;

    my_malloc_return:
    pthread_mutex_unlock(&__my_pool__.mutex);
    return mapaddr;
}

void my_free(void* ptr) {
    if (!__my_pool__.head) {
        return;
    }

    pthread_mutex_lock(&__my_pool__.mutex);
    struct __block__* this_block = (struct __block__*)((ptrdiff_t)ptr - sizeof(void*));

    // update next_free pointers of preceding blocks currently pointing to this one
    /* ... this is slow as hell actually, don't do this
    struct __block__* prev = this_block - 1;
    while (prev >= __my_pool__.head) {
        if (prev->next_free < this_block->next_free) break;
        if (prev->next_free == this_block->next_free) {
            prev->next_free = this_block;
        }
        prev--;
    }
    if (this_block < __my_pool__.next_free) {
        __my_pool__.next_free = this_block;
    } */
    struct __block__* new_next_free = __my_pool__.next_free;
    /* this was a mistake, but still worked fine. I'm confused.
    this_block->next_free = new_next_free;
    __my_pool__.next_free = this_block; */
    __my_pool__.next_free = this_block;
    this_block->next_free = new_next_free;

    pthread_mutex_unlock(&__my_pool__.mutex);
}
