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
#include <stdatomic.h>

#include "common.h"

#define MAX_CLIENTS 1024

#define THREADCATCH(exit_cond, ...)         \
    do {                                    \
        if(exit_cond) {                     \
            fprintf(stderr, __VA_ARGS__);   \
            close(client_fd);               \
            return NULL;                    \
        }                                   \
    } while(0)

int sock, signum;
atomic_int num_connections;

pthread_t pt_listen;
pthread_t pt_clients[MAX_CLIENTS];
size_t last_client = -1;

void exit_cleanup(int status) {
    close(sock);

    // join all client threads that were registered before shutdown
    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        if (pt_clients[i]) {
            pthread_join(pt_clients[i], NULL);
        }
    }

    exit(status);
}

void sigh(int sig) {
    signum = sig;
    pthread_cancel(pt_listen);
}

void* chat_worker(void* arg) {
    int client_fd = (int)(uintptr_t)arg;

    char client_name[MAX_NAME_LEN];
    int name_length = read(client_fd, client_name, MAX_NAME_LEN);
    name_length -= 2;
    client_name[name_length] = '\0';
    THREADCATCH(name_length < 0, "Socket error or connection closed\n");
    
    printf("%s connected.\n", client_name);

    bool connection_closed = false;
    while (1) {
        dprintf(client_fd, "> ");

        char buffer[BUF_SIZE] = { '\0' };
        int bytes_read = read(client_fd, buffer, BUF_SIZE);
        THREADCATCH(bytes_read == -1, "Error reading from client socket: %s\n", strerror(errno));

        connection_closed = (bytes_read == 0) || BUF_MATCH(CTRL_C);

        // ignore empty lines
        if (BUF_MATCH("\n") || BUF_MATCH(CRLF)) {
            continue;
        }
        // handle commands
        else if (buffer[0] == '/') {
            if (BUF_MATCHCMD("quit")) {
                connection_closed = true;
            } else if (BUF_MATCHCMD("shutdown")) {
                connection_closed = true;
                pthread_cancel(pt_listen);
            }
        }

        if (connection_closed) {
            printf("%s disconnected.\n", client_name);
            break;
        }

        printf("%s: %s", client_name, buffer);
    }

    atomic_fetch_sub(&num_connections, 1);
    CATCH(close(client_fd) == -1, "Error closing client socket: %s\n", strerror(errno));

    return NULL;
}

void* listener_thread(void* arg) {
    int port = *(int*)arg;

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
    int t = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));
    CATCH(bind(sock, (struct sockaddr*)&addr, addrlen) == -1, "Error binding to port %d: %s\n", port, strerror(errno));
    CATCH(listen(sock, 0) == -1, "Error listening on port %d: %s\n", port, strerror(errno));
    printf("Listening on port %d\n", port);

    pthread_attr_t client_attr;
    pthread_attr_init(&client_attr);
    while (1) {
        int client_fd = accept(sock, (struct sockaddr*)&addr, &addrlen);
        CATCH(client_fd == -1, "Error accepting connection: %s\n", strerror(errno));

        last_client++;
        if (last_client == MAX_CLIENTS) {
            fprintf(stderr, "Connection limit exceeded\n");
            dprintf(client_fd, "Connection limit exceeded\r\n");
            close(client_fd);
            continue;
        }

        atomic_fetch_add(&num_connections, 1);
        pthread_create(&pt_clients[last_client], &client_attr, &chat_worker, (void*)(uintptr_t)client_fd);
    }

    return NULL;
}

int main(int argc, char const* argv[]) {
    int exit_status = EXIT_SUCCESS;
    int port = PORT;
    if (argc >= 2) {
        char* end;
        port = strtol(argv[1], &end, 10);
        CATCH(end != (argv[1] + strlen(argv[1])), "Usage: %s [port]\n", argv[0]);
    }

    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    CATCH(sigaction(SIGINT, &act, NULL) == -1, "Error binding signal handler for SIGINT: %s\n", strerror(errno));
    CATCH(sigaction(SIGTERM, &act, NULL) == -1, "Error binding signal handler for SIGTERM: %s\n", strerror(errno));

    // ignore pipe errors
    struct sigaction act_ign = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART
    };
    CATCH(sigaction(SIGPIPE, &act_ign, NULL) == -1, "Error binding signal handler for SIGPIPE: %s\n", strerror(errno));

    atomic_init(&num_connections, 0);

    // create the listener thread
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_create(&pt_listen, &pt_attr, &listener_thread, &port);

    // wait for the listener thread to exit, then clean up after all clients have exited
    pthread_join(pt_listen, NULL);

    int remaining = atomic_load(&num_connections);
    if (signum) {
        fprintf(stderr, "Caught %s, shutting down immediately.\n", strsignal(signum));
        exit_status = EXIT_FAILURE;
    } else if (remaining > 0) {
        char wait_msg[11];
        snprintf(wait_msg, 11, (remaining > 1 ? "%d clients" : "one client"), remaining);
        printf("Server is shutting down. Waiting for %s to disconnect...\n", wait_msg);
        while (atomic_load(&num_connections) > 0) {
            sleep(1);
        }
    }

    exit_cleanup(exit_status);
}
