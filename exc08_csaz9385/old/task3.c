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
#include "thread_pool.h"

#define PORT 8080
#define WORKER_COUNT 16
#define MAX_JOB_COUNT 1000

#define THREADCATCH(exit_cond, ...)         \
    do {                                    \
        if(exit_cond) {                     \
            fprintf(stderr, __VA_ARGS__);   \
            close(client_fd);               \
            return NULL;                    \
        }                                   \
    } while(0)

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            exit(EXIT_FAILURE);                     \
        }                                           \
    } while(0)

int sock;
thread_pool pool;
pthread_t listener_thread;
job_id jobs[MAX_JOB_COUNT];
int num_jobs = 0;

void exit_cleanup(int status, int signum) {
    char buffer[64];
    if (signum) {
        snprintf(buffer, 64, "Caught %s\n", strsignal(signum));
        write(STDERR_FILENO, buffer, strlen(buffer));
    }
    snprintf(buffer, 64, "Shutting down...\n");
    write(STDERR_FILENO, buffer, strlen(buffer));

    close(sock);

    for (int i = 0; i < num_jobs; i++) {
        pool_await(jobs[i]);
    }
    pool_destroy(&pool);
    exit(status);
}

void sigh(int signum) {
    exit_cleanup(EXIT_FAILURE, signum);
}

#define BUF_SIZE 1500
void* http_worker(void* arg) {
    usleep(100000);
    int client_fd = (int)(uintptr_t)arg;

    char* headers[] = {
        "HTTP/1.1 200 OK\r\n",
        "Content-Type: text/html\r\n",
        "Content-Length: %d\r\n",
        "\r\n"
    };

    char body[256];
    snprintf(body, 100, "Testing text shinies<br>\r\nYet Another Line<br>\r\nYou were served by: %lx\r\n", pthread_self());
    int content_length = strlen(body);

    char buffer[BUF_SIZE] = { '\0' };
    int bytes_read = read(client_fd, buffer, BUF_SIZE);
    THREADCATCH(bytes_read == -1, "Error reading from client socket: %s\n", strerror(errno));

    THREADCATCH(dprintf(client_fd, headers[0]) < 0, "Error writing to client socket\n");
    THREADCATCH(dprintf(client_fd, headers[1]) < 0, "Error writing to client socket\n");
    THREADCATCH(dprintf(client_fd, headers[2], content_length) < 0, "Error writing to client socket\n");
    THREADCATCH(dprintf(client_fd, headers[3]) < 0, "Error writing to client socket\n");
    THREADCATCH(dprintf(client_fd, body) < 0, "Error writing to client socket\n");

    if (strstr(buffer, "GET /shutdown")) {
        pthread_cancel(listener_thread);        
    }

    CATCH(close(client_fd) == -1, "Error closing client socket: %s\n", strerror(errno));

    return NULL;
}

void* listener(void* arg) {
    (void)arg;
    
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CATCH(sock == -1, "Error creating socket: %s", strerror(errno));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = htonl(INADDR_ANY)
    };
    socklen_t addrlen = (socklen_t)sizeof(addr);
    int t = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));
    CATCH(bind(sock, (struct sockaddr*)&addr, addrlen) == -1, "Error binding socket: %s\n", strerror(errno));
    CATCH(listen(sock, 0) == -1, "Error listening on socket: %s\n", strerror(errno));

    while (1) {
        int client_sock = accept(sock, (struct sockaddr*)&addr, &addrlen);
        CATCH(client_sock == -1, "Error accepting connection: %s\n", strerror(errno));
        puts("Received connection");

        jobs[num_jobs++] = pool_submit(&pool, &http_worker, (job_arg)(uintptr_t)client_sock);
    }

    return NULL;
}

int main() {
    printf("Listening on port %d\n", PORT);

    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    CATCH(sigaction(SIGINT, &act, NULL) == -1, "Error binding signal handler for SIGINT: %s\n", strerror(errno));
    CATCH(sigaction(SIGTERM, &act, NULL) == -1, "Error binding signal handler for SIGTERM: %s\n", strerror(errno));
    
    struct sigaction act_ign = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART
    };
    CATCH(sigaction(SIGPIPE, &act_ign, NULL) == -1, "Error binding signal handler for SIGPIPE: %s\n", strerror(errno));

    // create the listener thread
    pthread_attr_t pt_attr;
    pthread_attr_init(&pt_attr);
    pthread_create(&listener_thread, &pt_attr, &listener, NULL);

    // create the worker pool
    pool_create(&pool, WORKER_COUNT);

    // wait for the listener thread to exit, then cleanup
    pthread_join(listener_thread, NULL);
    exit_cleanup(EXIT_SUCCESS, 0);
}
