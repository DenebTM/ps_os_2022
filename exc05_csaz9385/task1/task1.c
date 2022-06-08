#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define SHM_NAME "/csaz9385_task1"
#define CATCH(exit_cond, ...)               \
    do {                                    \
        if(exit_cond) {                     \
            fprintf(stderr, __VA_ARGS__);   \
            exit(EXIT_FAILURE);             \
        }                                   \
    } while(0)

uint64_t *rbuf, *result;
uint64_t num_count, buf_length;

void producer() {
    for (uint64_t i = 0; i < num_count; i++) {
        rbuf[i % buf_length] = i+1;
    }

    exit(EXIT_SUCCESS);
}

void consumer() {
    *result = 0;
    for (uint64_t i = 0; i < num_count; i++) {
        *result += rbuf[i % buf_length];
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    CATCH(argc != 3, "Usage: %s N[int] B[int]\n", argv[0]);

    char *end;
    num_count = strtoull(argv[1], &end, 10);
    CATCH(end != argv[1]+strlen(argv[1]), "N must be an integer\n");
    buf_length = strtoull(argv[2], &end, 10);
    CATCH(end != argv[2]+strlen(argv[2]), "B must be an integer\n");

    int shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    CATCH(shmfd == -1, "Creating shared memory object failed\n");

    size_t shm_size = sizeof(*rbuf) * buf_length + sizeof(*result);
    ftruncate(shmfd, shm_size);
    rbuf = mmap(NULL, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
    CATCH(rbuf == MAP_FAILED, "Mapping shared memory object failed\n");
    result = rbuf + buf_length;

    pid_t cpid1 = fork();
    CATCH(cpid1 == -1, "Spawning producer process failed\n");
    if (cpid1 == 0) {
        producer();
    }

    pid_t cpid2 = fork();
    CATCH(cpid2 == -1, "Spawning consumer process failed\n");
    if (cpid2 == 0) {
        consumer();
    }

    wait(NULL);
    wait(NULL);
    printf("Result: %lu\n", *result);

    munmap(rbuf, shm_size);
    close(shmfd);
    shm_unlink(SHM_NAME);
    return EXIT_SUCCESS;
}

/*  For small inputs, the result is the expected sum of numbers. However, with
    sufficiently large inputs, the result starts to become inconsistent and
    vary between runs. In this, the behaviour is similar to that of exc03/task1
    - with smaller inputs, the child that spawns first (the producer) gets most
    or all of its work done before the next (the consumer) tries to access
    values not written yet, but with larger ones, the consumer may begin to
    outpace the producer, leading to unexpected results.                     */
