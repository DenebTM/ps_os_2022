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
#include <errno.h>
#include <semaphore.h>

#define SHM_NAME "/csaz9385_task2"
#define CATCH(exit_cond, cleanup, ...)              \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit_cleanup(cleanup, EXIT_FAILURE);    \
        }                                           \
    } while(0)

int shmfd;
size_t shm_size;
uint64_t num_count, buf_length;
struct shared_mem {
    sem_t new_data;
    sem_t buffer_free;
    uint64_t result;
    uint64_t rbuf[];
} *shm;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
enum cleanup_level { NONE, SHM_ONLY, FULL };
void exit_cleanup(int level, int status) {
    switch (level) {
        case FULL:
            sem_destroy(&shm->new_data);
            sem_destroy(&shm->buffer_free);
        case SHM_ONLY:
            munmap(shm, shm_size);
            close(shmfd);
            shm_unlink(SHM_NAME);
    }
    exit(status);
}
#pragma GCC diagnostic pop

void producer() {
    for (uint64_t i = 0; i < num_count; i++) {
        sem_wait(&shm->buffer_free);
        shm->rbuf[i % buf_length] = i+1;
        sem_post(&shm->new_data);
    }

    exit(EXIT_SUCCESS);
}

void consumer() {
    shm->result = 0;
    for (uint64_t i = 0; i < num_count; i++) {
        sem_wait(&shm->new_data);
        shm->result += shm->rbuf[i % buf_length];
        sem_post(&shm->buffer_free);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    // Argument checking
    CATCH(argc != 3, 0, "Usage: %s N[int] B[int]\n", argv[0]);
    char *end;
    num_count = strtoull(argv[1], &end, 10);
    CATCH(end != argv[1]+strlen(argv[1]), NONE, "N must be an integer\n");
    buf_length = strtoull(argv[2], &end, 10);
    CATCH(end != argv[2]+strlen(argv[2]), NONE, "B must be an integer\n");

    // Create shared memory object and map it at *shm
    shmfd = shm_open(SHM_NAME, O_CREAT|O_RDWR, 0600);
    CATCH(shmfd == -1, NONE, "Creating shared memory object failed: %s\n", strerror(errno));

    shm_size = sizeof(*shm) + sizeof(*shm->rbuf) * (buf_length - 1);
    ftruncate(shmfd, shm_size);
    shm = mmap(NULL, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shmfd, 0);
    CATCH(shm == MAP_FAILED, SHM_ONLY, "Mapping shared memory object failed: %s\n", strerror(errno));

    // Initialize semaphores
    CATCH(sem_init(&shm->new_data, 1, 0) == -1, SHM_ONLY, "Creating semaphore 1 failed: %s\n", strerror(errno));
    CATCH(sem_init(&shm->buffer_free, 1, buf_length) == -1, FULL, "Creating semaphore 2 failed: %s\n", strerror(errno));

    // Create producer and consumer child processes
    pid_t cpid1 = fork();
    CATCH(cpid1 == -1, FULL, "Spawning producer process failed: %s\n", strerror(errno));
    if (cpid1 == 0) {
        producer();
    }
    pid_t cpid2 = fork();
    CATCH(cpid2 == -1, FULL, "Spawning consumer process failed: %s\n", strerror(errno));
    if (cpid2 == 0) {
        consumer();
    }

    // Wait for children to finish, print result, clean up and exit
    int wstatus;
    for(int i = 0; i < 2; i++) {
        int pid = wait(&wstatus);
        CATCH(pid == -1, FULL, "Error waiting for child process: %s\n", strerror(errno));

        char *cname = (pid == cpid1) ? "Producer" : "Consumer";
        if(WIFSIGNALED(wstatus)) {
            fprintf(stderr, "%s was terminated by %s\n", cname, strsignal(WTERMSIG(wstatus)));
            exit_cleanup(FULL, EXIT_FAILURE);
        } else if(WEXITSTATUS(wstatus) != EXIT_SUCCESS) {
            fprintf(stderr, "%s exited with status %d\n", cname, WEXITSTATUS(wstatus));
            exit_cleanup(FULL, EXIT_FAILURE);
        }
    }
    printf("Result: %lu\n", shm->result);
    exit_cleanup(FULL, EXIT_SUCCESS);
}

/* task1 finishes quicker, but the result is often incorrect, especially for
    large values of N. Additionally, the result is somewhat dependent on B.
   task2 takes a little longer, but returns the correct result every time.
    The time taken is somewhat dependent on B - very small values are
    especially slow, but you quickly run into diminishing returns.           */
