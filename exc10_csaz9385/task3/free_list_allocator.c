#include "free_list_allocator.h"

_Thread_local struct __free_list__ __my_pool__;

void my_allocator_destroy(void) {
    if (__my_pool__.head) {
        munmap(__my_pool__.head, __my_pool__.size);
        __my_pool__.head = NULL;
        __my_pool__.next_free = NULL;
    }
}

void my_allocator_init(size_t size) {
    // attempt to destroy the pool in case it was initialized before
    my_allocator_destroy();

    // allocate space for the linked list and note down the size of the pool
    __my_pool__.head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    __my_pool__.next_free = __my_pool__.head;
    __my_pool__.size = size;
    size /= sizeof(struct __block__);

    // assign pointers of every block
    for (size_t i = 0; i < size; i++) {
        if (i < size-1) __my_pool__.head[i].next_free = &__my_pool__.head[i+1];
    }
}

void* my_malloc(size_t size) {
    if (size > __BLOCK_SIZE__ || !__my_pool__.head) {
        return NULL;
    }

    struct __block__* this_block;
    if (!(this_block = __my_pool__.next_free)) {
        return NULL;
    }
    
    __my_pool__.next_free = this_block->next_free;

    return &this_block->mem;
}

void my_free(void* ptr) {
    if (!__my_pool__.head) {
        return;
    }

    struct __block__* this_block = (struct __block__*)((ptrdiff_t)ptr - sizeof(void*));
    struct __block__* new_next_free = __my_pool__.next_free;
    __my_pool__.next_free = this_block;
    this_block->next_free = new_next_free;
}
