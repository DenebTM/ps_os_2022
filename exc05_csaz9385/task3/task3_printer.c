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
            return EXIT_FAILURE;                    \
        }                                           \
    } while(0)

bool shall_exit = false;
mqd_t mqd;

void sigh() {
    shall_exit = true;
}

int main(int argc, char* argv[]) {
    CATCH(argc != 2, "Usage: %s priority[int]\n", argv[0]);

    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    char *end;
    unsigned int prio = strtoul(argv[1], &end, 10);
    CATCH(end != argv[1] + strlen(argv[1]), "Priority must be an integer\n");

    mqd = mq_open(MQ_NAME, O_WRONLY|O_NONBLOCK);
    CATCH(mqd == -1, "mq_open failed with error: %s\n", strerror(errno));

    int len;
    char c;
    char buf[BUF_SIZE+1] = { 0 };
    while ((c = getc(stdin)) != EOF && len < BUF_SIZE && !shall_exit) {
        buf[len++] = c;
    }
    buf[len] = '\0';

    int mq_status = mq_send(mqd, buf, len, prio);
    while (mq_status == -1 && !shall_exit) {
        CATCH(errno != EAGAIN, "mq_send failed with error: %s\n", strerror(errno));
        puts("Print server offline or not currently taking jobs. Retrying in 3 seconds...\n");
        sleep(3);
        mq_status = mq_send(mqd, buf, len, prio);
    }

    mq_close(mqd);
    return EXIT_SUCCESS;
}
