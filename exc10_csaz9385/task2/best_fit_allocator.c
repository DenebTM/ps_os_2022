#include "best_fit_allocator.h"

struct __free_list__ __my_pool__;

void my_allocator_destroy(void) {
    if (__my_pool__.head) {
        pthread_mutex_lock(&__my_pool__.mutex);
        munmap(__my_pool__.head, __my_pool__.size);
        __my_pool__.head = NULL;
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
    __my_pool__.size = size;

    // initialize first block's size
    __my_pool__.first_free = __my_pool__.head;
    __my_pool__.first_free->size = size - __BLOCK_HEADER_SIZE__;

    pthread_mutex_unlock(&__my_pool__.mutex);
}

void* my_malloc(size_t size) {
    if (!__my_pool__.first_free) {
        return NULL;
    }

    // do not allocate a block too small to hold a pointer
    size = size >= __BLOCK_MIN_SIZE__ ? size : __BLOCK_MIN_SIZE__;

    pthread_mutex_lock(&__my_pool__.mutex);
    void* mapaddr = NULL;

    // find best block to fulfill the request
    struct __block__* prev = NULL;
    struct __block__* block = __my_pool__.first_free;
    struct __block__* before_best_fit = NULL;
    struct __block__* best_fit = NULL;
    while (block) {
        if (block->size >= size &&
          (!best_fit || block->size - size < best_fit->size - size)) {
            best_fit = block;
            before_best_fit = prev;
        }

        prev = block;
        block = block->next_free;
    }
    if (!best_fit) {
        goto my_malloc_return;
    }

    // create a new block only if it could at least hold a pointer in its data area
    if (best_fit->size - size >= __BLOCK_HEADER_SIZE__ + __BLOCK_MIN_SIZE__) {
        struct __block__* new_block = (struct __block__*)((ptrdiff_t)&best_fit->mem + size);
        new_block->size = best_fit->size - size - __BLOCK_HEADER_SIZE__;
        new_block->next_free = best_fit->next_free;
        best_fit->next_free = new_block;
        best_fit->size = size;
    }
    // update the head's first_free pointer if the first node was used
    // otherwise, update the previous free block's next_free pointer
    if (before_best_fit) {
        before_best_fit->next_free = best_fit->next_free;
    } else {
        __my_pool__.first_free = best_fit->next_free;
    }
    mapaddr = &best_fit->mem;

    my_malloc_return:
    pthread_mutex_unlock(&__my_pool__.mutex);
    return mapaddr;
}

void my_free(void* ptr) {
    if (!__my_pool__.head) {
        return;
    }

    pthread_mutex_lock(&__my_pool__.mutex);

    struct __block__* this_block = (struct __block__*)((ptrdiff_t)ptr - __BLOCK_HEADER_SIZE__);

    // traverse the chain of free blocks from the beginning, until one is found
    //   that points to a later location in memory as its successor
    // insert the block to be freed at this position
    struct __block__* before_prev_inorder = NULL;
    struct __block__** prev_inorder = &__my_pool__.first_free;
    while (*prev_inorder && *prev_inorder < this_block) {
        before_prev_inorder = *prev_inorder;
        prev_inorder = &((*prev_inorder)->next_free);
    }
    struct __block__* new_next = *prev_inorder;
    *prev_inorder = this_block;
    this_block->next_free = new_next;

    // merge with the following block, if possible
    if ((struct __block__*)((ptrdiff_t)&(this_block->mem) + this_block->size) == new_next) {
        this_block->size += new_next->size + __BLOCK_HEADER_SIZE__;
        this_block->next_free = new_next->next_free;
    }
    // merge with the preceding block, if possible
    if (before_prev_inorder &&
      (struct __block__*)((ptrdiff_t)&(before_prev_inorder->mem) + before_prev_inorder->size) == this_block) {
        before_prev_inorder->size += this_block->size + __BLOCK_HEADER_SIZE__;
        before_prev_inorder->next_free = this_block->next_free;
    }

    pthread_mutex_unlock(&__my_pool__.mutex);
}
