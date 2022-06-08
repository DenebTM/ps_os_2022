#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define CATCH(exit_cond, go_to, ...)                \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit_status = EXIT_FAILURE;             \
            goto go_to;                             \
        }                                           \
    } while(0)

typedef struct pt_args {
    int* in;
    int in_count;
} pt_args;

void* sum_numbers(void* arg) {
    pt_args* args = arg;
    int sum = 0;

    for (int i = 0; i < args->in_count; i++) {
        sum += args->in[i];
    }

    pthread_exit((void*)sum);
}

int main(int argc, char** argv)
{
    int exit_status = EXIT_SUCCESS;
    CATCH(argc < 2, exit_only, "Usage: %s num1 [num2 num3 ...]\n", argv[0]);
    int pt_count = argc-1;

    // pthread boilerplate
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_t* pts = malloc(sizeof(pthread_t) * pt_count);
    CATCH(pts == NULL, exit_only, "malloc pts failed: %s\n", strerror(errno));

    int* argv_ints = malloc(sizeof(int) * pt_count);
    CATCH(pts == NULL, cleanup_pts, "malloc argv_ints failed: %s\n", strerror(errno));
    pt_args* thread_args = malloc(sizeof(pt_args) * pt_count);
    CATCH(pts == NULL, cleanup_ints_pts, "malloc thread_args failed: %s\n", strerror(errno));

    // initialize data needed by threads
    for (int i = 0; i < pt_count; i++) {
        char* end;
        argv_ints[i] = strtol(argv[i+1], &end, 10);
        CATCH(end != argv[i+1] + strlen(argv[i+1]),
            cleanup_full,
            "%s is not an integer", argv[i]);

        thread_args[i] = (pt_args) {
            .in = argv_ints,
            .in_count = i + 1
        };
    }

    // start threads
    for (int i = 0; i < pt_count; i++) {
        CATCH(pthread_create(&pts[i], &pt_attr, &sum_numbers, &thread_args[i]) != 0,
            cleanup_full,
            "Error on pthread_create: %s\n", strerror(errno));
    }

    // wait for threads to finish and print their results
    for (int i = 0; i < pt_count; i++) {
        int thread_result;
        CATCH(pthread_join(pts[i], (void*)&thread_result) != 0,
            cleanup_full,
            "Error on pthread_join: %s\n", strerror(errno));
        printf("sum%d = %d\n", i+1, thread_result);
    }
    
    cleanup_full:
    free(thread_args);
    cleanup_ints_pts:
    free(argv_ints);
    cleanup_pts:
    free(pts);
    exit_only:
    return exit_status;
}
