#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "common.h"

int client_fd;

void exit_cleanup(int status) {
    close(client_fd);
    exit(status);
}

void sigh(int sig) {
    char* msg;
    switch (sig) {
        case SIGPIPE:
            msg = "Connection closed by server, exiting.\n";
            break;
        default:
            msg = "Caught signal, exiting.\n";
            write(client_fd, "/quit" CRLF, 7);
    }
    write(STDOUT_FILENO, msg, strlen(msg));
    exit_cleanup(EXIT_FAILURE);
}

int main(int argc, char const* argv[]) {
    CATCH(argc < 2, "Usage: %s [port] username\n", argv[0]);
    char username[MAX_NAME_LEN + 2] = { 0 };
    int port = PORT;
    if (argc == 2) {
        strncpy(username, argv[1], MAX_NAME_LEN);
    } else {
        char* end;
        port = strtol(argv[1], &end, 10);
        CATCH(end != (argv[1] + strlen(argv[1])), "Usage: %s [port]\n", argv[0]);

        strncpy(username, argv[2], MAX_NAME_LEN);
    }
    strcpy(username + strlen(username), CRLF);

    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    CATCH(sigaction(SIGINT, &act, NULL) == -1, "Error binding signal handler for SIGINT: %s\n", strerror(errno));
    CATCH(sigaction(SIGTERM, &act, NULL) == -1, "Error binding signal handler for SIGTERM: %s\n", strerror(errno));
    CATCH(sigaction(SIGPIPE, &act, NULL) == -1, "Error binding signal handler for SIGPIPE: %s\n", strerror(errno));

    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CATCH(client_fd == -1, "Error creating socket: %s", strerror(errno));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = htonl(INADDR_LOOPBACK)
    };
    socklen_t addrlen = (socklen_t)sizeof(addr);

    CATCH(connect(client_fd, (struct sockaddr*)&addr, addrlen) == -1, "Error opening socket connection: %s\n", strerror(errno));
    write(client_fd, username, strlen(username));

    bool exit = false;
    while (!exit) {
        char buffer[BUF_SIZE] = { 0 };
        printf("> ");
        fflush(stdout);
        CATCH(read(STDIN_FILENO, &buffer, BUF_SIZE) == -1, "Error reading from stdin: %s\n", strerror(errno));

        // handle commands
        if (buffer[0] == '/') {
            if ((BUF_MATCHCMD("quit") || BUF_MATCHCMD("shutdown"))) {
                exit = true;
            }
        }

        CATCH(write(client_fd, buffer, strlen(buffer)) == -1, "Error sending message: %s\n", strerror(errno));
    }

    exit_cleanup(EXIT_SUCCESS);
}
