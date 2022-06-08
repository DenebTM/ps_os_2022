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

#define MAX_CONCURRENT_CLIENTS 1024

#define THREADCATCH(exit_cond, ...)         \
    do {                                    \
        if(exit_cond) {                     \
            fprintf(stderr, __VA_ARGS__);   \
            close(client_fd);               \
            return NULL;                    \
        }                                   \
    } while(0)

#define BROADCAST(...)                                  \
    do {                                                \
        pthread_mutex_lock(&write_mutex);               \
        printf(__VA_ARGS__);                            \
        printf("\n");                                   \
        for (int i = 0; i < MAX_CONCURRENT_CLIENTS; i++) {         \
            if (clients[i].fd) {                        \
                dprintf(clients[i].fd, __VA_ARGS__);    \
            }                                           \
        }                                               \
        pthread_mutex_unlock(&write_mutex);             \
    } while (0)

#define BROADCAST_OTHER(...)                                    \
    do {                                                        \
        pthread_mutex_lock(&write_mutex);                       \
        printf(__VA_ARGS__);                                    \
        printf("\n");                                           \
        for (int i = 0; i < MAX_CONCURRENT_CLIENTS; i++) {                 \
            if (clients[i].fd && clients[i].fd != client_fd) {  \
                dprintf(clients[i].fd, __VA_ARGS__);            \
            }                                                   \
        }                                                       \
        pthread_mutex_unlock(&write_mutex);                     \
    } while (0)

int sock, signum;
atomic_int num_connections;

pthread_t pt_listen;
size_t last_client = 0;

pthread_mutex_t write_mutex;

struct client_data {
    int fd;
    pthread_t pt;
    char name[MAX_NAME_LEN+1];
} clients[MAX_CONCURRENT_CLIENTS];

void exit_cleanup(int status) {
    pthread_mutex_destroy(&write_mutex);
    close(sock);

    // join all client threads that were registered before shutdown
    for (size_t i = 0; i < MAX_CONCURRENT_CLIENTS; i++) {
        if (clients[i].pt) {
            pthread_join(clients[i].pt, NULL);
        }
    }

    exit(status);
}

void sigh(int sig) {
    signum = sig;
    pthread_cancel(pt_listen);
}

void* chat_worker(void* arg) {
    struct client_data* clptr = (struct client_data*)arg;
    int client_fd = clptr->fd;

    int name_length = read(client_fd, clptr->name, MAX_NAME_LEN);
    THREADCATCH(name_length <= 0, "Connection closed by server\n");

    // remove trailing CRLF (for telnet connections)
    if (strstr(clptr->name, CRLF))
        name_length -= 2;
    clptr->name[name_length] = '\0';

    dprintf(client_fd, CRLF);
    BROADCAST_OTHER("%s connected", clptr->name);

    bool connection_closed = false;
    while (!connection_closed) {
        char buffer[BUF_SIZE] = { '\0' };
        int bytes_read = read(client_fd, buffer, BUF_SIZE);
        THREADCATCH(bytes_read == -1, "Error reading from client socket: %s\n", strerror(errno));

        if (bytes_read == 0 || BUF_MATCH(CTRL_C)) {
            connection_closed = true;
            break;
        }

        // remove trailing CRLF (for telnet connections)
        if (strstr(buffer, CRLF))
            bytes_read -= 2;
        buffer[bytes_read] = '\0';

        // ignore empty lines
        if (BUF_MATCH("") || BUF_MATCH("\n") || BUF_MATCH(CRLF)) {
            continue;
        }

        // handle commands
        if (buffer[0] == '/') {
            if (BUF_MATCHCMD("quit")) {
                connection_closed = true;
            } else if (BUF_MATCHCMD("shutdown")) {
                connection_closed = true;
                pthread_cancel(pt_listen);
            } else {
                buffer[strcspn(buffer, " ")] = '\0';
                dprintf(client_fd, "Unknown command: \"%s\"", buffer);
            }
        } else {
            BROADCAST("%s: %s", clptr->name, buffer);
        }

    }
    BROADCAST_OTHER("%s disconnected", clptr->name);

    atomic_fetch_sub(&num_connections, 1);
    CATCH(close(client_fd) == -1, "Error closing client socket: %s\n", strerror(errno));
    clptr->fd = 0;

    pthread_exit(NULL);
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

        // find first free client slot
        for (last_client = 0; last_client < MAX_CONCURRENT_CLIENTS; last_client++) {
            if (!clients[last_client].fd) {
                // if there was a thread here before, join it to clean up
                if (clients[last_client].pt) {
                    pthread_join(clients[last_client].pt, NULL);
                }
                break;
            }
            last_client++;
        }
        if (last_client == MAX_CONCURRENT_CLIENTS) {
            fprintf(stderr, "Concurrent connection limit exceeded\n");
            dprintf(client_fd, "Concurrent connection limit exceeded, please try again later\r\n");
            close(client_fd);
            continue;
        }

        clients[last_client].fd = client_fd;
        atomic_fetch_add(&num_connections, 1);
        pthread_create(&clients[last_client].pt, &client_attr, &chat_worker, (void*)&clients[last_client]);
    }

    pthread_exit(NULL);
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

    pthread_mutexattr_t write_mutexattr;
    pthread_mutexattr_init(&write_mutexattr);
    pthread_mutex_init(&write_mutex, &write_mutexattr);

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
        BROADCAST("Server is shutting down. Waiting for %s to disconnect...", wait_msg);
        while (atomic_load(&num_connections) > 0) {
            sleep(1);
        }
    }

    exit_cleanup(exit_status);
}
