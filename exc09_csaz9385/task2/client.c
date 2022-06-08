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
#include <pthread.h>

#include "common.h"

pthread_t pt_recv;
bool client_exit;
int sock;

void exit_cleanup(int status) {
    close(sock);
    pthread_join(pt_recv, NULL);
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
    }
    write(STDOUT_FILENO, msg, strlen(msg));
    exit_cleanup(EXIT_FAILURE);
}

void* recv_thread(void* arg) {
    (void)arg;

    while (!client_exit) {
        char buffer[BUF_SIZE] = { 0 };
        int bytes_read = read(sock, buffer, BUF_SIZE);
        if (bytes_read == 0) {
            break;
        }

        printf("\r%s\n> ", buffer);
        fflush(stdout);
    }

    pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {
    CATCH(argc < 2, "Usage: %s [port] username\n", argv[0]);
    char username[MAX_NAME_LEN + 1] = { 0 };
    int port = PORT;
    if (argc == 2) {
        strncpy(username, argv[1], MAX_NAME_LEN);
    } else {
        char* end;
        port = strtol(argv[1], &end, 10);
        CATCH(end != (argv[1] + strlen(argv[1])), "Usage: %s [port]\n", argv[0]);

        strncpy(username, argv[2], MAX_NAME_LEN);
    }

    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    CATCH(sigaction(SIGINT, &act, NULL) == -1, "Error binding signal handler for SIGINT: %s\n", strerror(errno));
    CATCH(sigaction(SIGTERM, &act, NULL) == -1, "Error binding signal handler for SIGTERM: %s\n", strerror(errno));
    CATCH(sigaction(SIGPIPE, &act, NULL) == -1, "Error binding signal handler for SIGPIPE: %s\n", strerror(errno));

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CATCH(sock == -1, "Error creating socket: %s", strerror(errno));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = htonl(INADDR_LOOPBACK)
    };
    socklen_t addrlen = (socklen_t)sizeof(addr);

    CATCH(connect(sock, (struct sockaddr*)&addr, addrlen) == -1, "Error opening socket connection: %s\n", strerror(errno));

    // send username, await confirmation
    write(sock, username, strlen(username));
    char confirm_buffer[8];
    int bytes_read = read(sock, confirm_buffer, 8);
    CATCH(bytes_read <= 0, "Connection closed by server\n");
    
    // create the listener thread
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_create(&pt_recv, &pt_attr, &recv_thread, &port);

    // start relaying input to server
    while (!client_exit) {
        printf("\33[2K> ");
        fflush(stdout);

        // read input
        char buffer[BUF_SIZE] = { 0 };
        ssize_t bytes_read = read(STDIN_FILENO, &buffer, BUF_SIZE);
        CATCH(bytes_read == -1, "Error reading from stdin: %s\n", strerror(errno));
        buffer[bytes_read-1] = '\0';

        client_exit = (bytes_read == 0);

        // handle commands
        if (buffer[0] == '/') {
            if ((BUF_MATCHCMD("quit") || BUF_MATCHCMD("shutdown"))) {
                client_exit = true;
            }
        }
        
        if (!client_exit) {
            // move the cursor back to the previous line (awaiting the server echoing what we just typed)
            printf("\33[A");
        }
        CATCH(dprintf(sock, buffer) == -1, "Error sending message: %s\n", strerror(errno));
    }

    exit_cleanup(EXIT_SUCCESS);
}
