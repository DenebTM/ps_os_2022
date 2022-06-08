#include <stdio.h>
#include <stdlib.h>

#include "../allocator_tests.h"
#include "../membench.h"

#ifdef FREE_LIST
#include "best_fit_allocator.h"
#elif BEST_FIT
#include "free_list_allocator.h"
#endif

int main()
{
    #ifdef TEST
    #ifdef FREE_LIST
    test_free_list_allocator();
    #elif BEST_FIT
    test_best_fit_allocator();
    #endif
    #endif

    #ifdef BENCHMARK
    run_membench_thread_local(&my_allocator_init, &my_allocator_destroy, &my_malloc, &my_free);
    #endif

    return EXIT_SUCCESS;
}
