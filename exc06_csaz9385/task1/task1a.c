#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
          /*exit_cleanup(cleanup, EXIT_FAILURE);*/  \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

int my_global = 1;

void* var_increment(void* var) {
    (*(int*)var)++;
    return var;
}

int main()
{
    printf("%d\n", my_global);

    // child process
    pid_t cpid = fork();
    CATCH(cpid == -1, "Error on fork: %s\n", strerror(errno));
    if (cpid == 0) {
        var_increment(&my_global);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    printf("%d\n", my_global);

    // pthread
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_t pt = 0;
    CATCH(pthread_create(&pt, &pt_attr, &var_increment, &my_global) != 0, "Error on pthread_create: %s\n", strerror(errno));
    CATCH(pthread_join(pt, NULL) != 0, "Error on pthread_join: %s\n", strerror(errno));
    printf("%d\n", my_global);

    return EXIT_SUCCESS;
}
