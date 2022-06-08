#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <limits.h>

#define FIFO_PREFIX "/tmp/csaz9385_exc04_"
#define FIFO_MAXLEN 40

int client_fd;
char *buf;

void exit_cleanup() {
    printf("Exiting...\n");
    close(client_fd);
    free(buf);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [client name]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char fname[FIFO_MAXLEN];
    snprintf(fname, FIFO_MAXLEN, FIFO_PREFIX "%s", argv[1]);

    client_fd = open(fname, O_WRONLY);
    if (client_fd == -1) {
        fprintf(stderr, "Failed to open FIFO %s\n", fname);
        return EXIT_FAILURE;
    }

    write(client_fd, "\02", 1);
    printf("Connected\n");

    signal(SIGTERM, &exit_cleanup);
    signal(SIGINT, &exit_cleanup);

    buf = malloc(PIPE_BUF * sizeof(char));
    buf[0] = '\0';
    ssize_t num_bytes;
    while (buf[0] != '\n') {
        num_bytes = read(STDIN_FILENO, buf, PIPE_BUF);
        buf[num_bytes] = '\0';
        write(client_fd, buf, num_bytes+1);
        usleep(10000);
    }
    exit_cleanup();
}
