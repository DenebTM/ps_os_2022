#include <stdio.h>
#include <stdlib.h>

#include "best_fit_allocator.h"
#include "../allocator_tests.h"
#include "../membench.h"

int main()
{
    #ifdef TEST
    test_best_fit_allocator();
    #endif

    #ifdef BENCHMARK
    run_membench_global(&my_allocator_init, &my_allocator_destroy, &my_malloc, &my_free);
    #endif

    return EXIT_SUCCESS;
}
