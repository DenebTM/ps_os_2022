#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit_status = EXIT_FAILURE;             \
            exit_cleanup();                         \
        }                                           \
    } while(0)

int exit_status = EXIT_SUCCESS;
bool shall_exit = false;
mqd_t mqd;

void sigh() {
    exit_status = EXIT_FAILURE;
    shall_exit = true;
}

void exit_cleanup() {
    puts("Shutting down\n");
    mq_close(mqd);
    mq_unlink(MQ_NAME);
    exit(exit_status);
}

void print_msg(char* buf) {
    int i = 0;
    char c;
    while ((c = buf[i++]) != '\0') {
        putc(c, stdout);
        fflush(stdout);
        usleep(20000);
    }
    if (buf[i-2] != '\n') {
        putc('\n', stdout);
    }
}

int main(void) {
    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    mq_unlink(MQ_NAME);
    mqd = mq_open(MQ_NAME, O_RDONLY|O_CREAT|O_NONBLOCK, 0600, NULL);
    CATCH(mqd == -1, "mq_open failed with error: %s\n", strerror(errno));

    char buf[BUF_SIZE+1] = { 0 };
    while(!shall_exit) {
        unsigned int prio = 99;
        ssize_t recvbytes = mq_receive(mqd, buf, BUF_SIZE+1, &prio);
        CATCH(recvbytes == -1 && errno != EAGAIN, "mq_receive failed with error: %s\n", strerror(errno));

        buf[recvbytes] = '\0';
        if (recvbytes > 0) {
            printf("Serving print job with priority %d:\n", prio);
            print_msg(buf);
        }

        usleep(100000);
    }
    exit_cleanup();
}
