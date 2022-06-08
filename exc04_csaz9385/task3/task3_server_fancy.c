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
#include <string.h>

#define FIFO_PREFIX "/tmp/csaz9385_exc04_"
#define FIFO_MAXLEN 40
#define FIFO_FLAGS O_RDONLY | O_NONBLOCK

char **client_names;
char **fifo_names;
struct pollfd *fifo_pollfds;
int num_clients, num_successful;
char *buf;

int create_client_fifos(char ** const names, const int num_clients) {
    for (int i = 0; i < num_clients; i++) {
        num_successful = i;
        snprintf(fifo_names[i], FIFO_MAXLEN, FIFO_PREFIX "%s", names[i]);
        if (mkfifo(fifo_names[i], 0600) == -1) {
            fprintf(stderr, "Failed to create FIFO at %s\n", fifo_names[i]);
            return num_successful;
        }
        int fd = open(fifo_names[i], FIFO_FLAGS);
        if (fd == -1) {
            fprintf(stderr, "Failed to open FIFO at %s\n", fifo_names[i]);
            return num_successful;
        }
        fifo_pollfds[i].fd = fd;
        fifo_pollfds[i].events = POLLIN | POLLERR | POLLHUP;
        fifo_pollfds[i].revents = 0;
    }

    return num_successful = num_clients;
}

void delete_client_fifos(const int num_clients) {
    for (int i = 0; i < num_clients; i++) {
        close(fifo_pollfds[i].fd);
        remove(fifo_names[i]);
    }
}

void exit_cleanup() {
    printf("Exiting...\n");
    delete_client_fifos(num_clients);
    free(fifo_names);
    free(fifo_pollfds);
    free(buf);

    exit(EXIT_SUCCESS);
}

void rem_whitespace(char *str) {
    int in_ptr = 0, out_ptr = 0;
    for (; str[in_ptr]; in_ptr++) {
        if (str[in_ptr] == ' '  || str[in_ptr] == '\t' ||
            str[in_ptr] == '\n' || str[in_ptr] == '\r') {
            continue;
        }
        str[out_ptr++] = str[in_ptr];
    }
    str[out_ptr] = '\0';
}

int parse_expr(double *result, char *expr) {
    double num1, num2;
    int offset;

    if        (sscanf(expr, "%lf+%lf%n", &num1, &num2, &offset) == 2) {
        *result = num1+num2;
    } else if (sscanf(expr, "%lf-%lf%n", &num1, &num2, &offset) == 2) {
        *result = num1-num2;
    } else if (sscanf(expr, "%lf*%lf%n", &num1, &num2, &offset) == 2) {
        *result = num1*num2;
    } else if (sscanf(expr, "%lf/%lf%n", &num1, &num2, &offset) == 2) {
        *result = num1/num2;
    } else {
        return -1;
    }

    // spurious characters after the expression
    if (buf[offset] != '\0') {
        return -1;
    }
    return 0;
}

int main(int argc, char ** const argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [client name] [client names...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_clients = argc - 1;
    client_names = &argv[1];
    fifo_names = calloc(num_clients, sizeof(char*));
    for (int i = 0; i < num_clients; i++) {
        fifo_names[i] = malloc(sizeof(char) * FIFO_MAXLEN);
    }
    fifo_pollfds = calloc(num_clients, sizeof(struct pollfd));

    signal(SIGTERM, &exit_cleanup);
    signal(SIGINT, &exit_cleanup);

    puts("Ready to accept client connections...");

    create_client_fifos(client_names, num_clients);
    if (num_successful != num_clients) {
        delete_client_fifos(num_successful);
        return EXIT_FAILURE;
    }

    buf = malloc(PIPE_BUF * sizeof(char));
    while (1) {
        for (int i = 0; i < num_clients; i++) {
            if(poll(&fifo_pollfds[i], 1, 0) == 1) {
                switch(fifo_pollfds[i].revents) {
                    case POLLIN:
                        read(fifo_pollfds[i].fd, buf, PIPE_BUF);
                        // This byte is sent immediately after a client connects,
                        //  so it is used as an indication that the connection is new
                        if (buf[0] == '\02') {
                            printf("%s: Client connected\n", client_names[i]);
                        } else if (buf[0] != '\n') {
                            rem_whitespace(buf);
                            double result;
                            if (parse_expr(&result, buf) == -1) {
                                printf("%s: Malformed expression\n", client_names[i]);
                            } else {
                                printf("%s: %s = %lf\n", client_names[i], buf, result);
                            }
                        }
                        break;
                    case POLLHUP:
                        printf("%s: Client disconnected\n", client_names[i]);
                        close(fifo_pollfds[i].fd);
                        fifo_pollfds[i].fd = open(fifo_names[i], FIFO_FLAGS);
                        break;
                }
                fifo_pollfds[i].revents = 0;
            }
        }
        usleep(10000);
    }

    exit_cleanup();
}
