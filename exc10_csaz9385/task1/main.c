#include <stdio.h>
#include <stdlib.h>

#include "free_list_allocator.h"
#include "../allocator_tests.h"
#include "../membench.h"

int main()
{
    #ifdef TEST
    test_free_list_allocator();
    #endif

    #ifdef BENCHMARK
    run_membench_global(&my_allocator_init, &my_allocator_destroy, &my_malloc, &my_free);
    #endif

    return EXIT_SUCCESS;
}
