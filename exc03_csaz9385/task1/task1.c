#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define ERR_ARGC -1
#define ERR_ARGV -2

double mc_pi(int64_t S) {
    int64_t in_count = 0;
    for(int64_t i = 0; i < S; ++i) {
        const double x = rand() / (double)RAND_MAX;
        const double y = rand() / (double)RAND_MAX;
        if(x*x + y*y <= 1.f) {
            in_count++;
        }
    }
    return 4 * in_count / (double)S;
}

void print_usage(char* progname) {
    printf("Usage: %s (number of children) (number of samples)\n", progname);
}

int main(int argc, char* argv[]) {
    double result = 0;
    int childnum = -1;
    
    if (argc < 3) {
        print_usage(argv[0]);
        return ERR_ARGC;
    }
    
    char *end1, *end2;
    int nchildren = strtol(argv[1], &end1, 10),
        nsamples  = strtol(argv[2], &end2, 10);
        
    if (end1 != argv[1] + strlen(argv[1])
     || end2 != argv[2] + strlen(argv[2])) {
        print_usage(argv[0]);
        return ERR_ARGV;
    }

    for (int i = 0; i < nchildren; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            childnum = i;
            break;
        }
    }

    if (childnum != -1) {
        srand(getpid());
        result = mc_pi(nsamples);
        printf("Child %d PID = %d, mc_pi(%d) = %f\n", childnum, getpid(), nsamples, result);
    }
    else {
        for (int i = 0; i < nchildren; i++) {
            wait(NULL);
        }
        puts("Done.\n");
    }

    return EXIT_SUCCESS;
}
