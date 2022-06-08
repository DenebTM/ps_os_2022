#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define PORT 8023

#define CATCH(exit_cond, ...)                       \
    do {                                            \
        if(exit_cond) {                             \
            fprintf(stderr, __VA_ARGS__);           \
            return EXIT_FAILURE;                    \
        }                                           \
    } while(0)

int exit_status = EXIT_SUCCESS;
int client_fd, client_sock;

void sigh(int signum) {
    char buffer[64];
    snprintf(buffer, 64, "Caught %s, shutting down...\n", strsignal(signum));
    write(STDERR_FILENO, buffer, strlen(buffer));
    close(client_sock);
    close(client_fd);
    exit(EXIT_FAILURE);
}

int main() {
    printf("Listening on port %d\n", PORT);

    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CATCH(client_fd == -1, "Error creating socket: %s", strerror(errno));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = htonl(INADDR_ANY)
    };
    socklen_t addrlen = (socklen_t)sizeof(addr);
    int t = 1;
    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

    CATCH(bind(client_fd, (struct sockaddr*)&addr, addrlen) == -1, "Error binding socket: %s\n", strerror(errno));
    CATCH(listen(client_fd, 0) == -1, "Error listening on socket: %s\n", strerror(errno));
    
    // catch 'kill' and Ctrl+C to run clean-up tasks before exiting
    struct sigaction act = {
        .sa_handler = &sigh,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);
    CATCH(sigaction(SIGINT, &act, NULL) == -1, "Error binding signal handler for SIGINT: %s\n", strerror(errno));
    CATCH(sigaction(SIGTERM, &act, NULL) == -1, "Error binding signal handler for SIGTERM: %s\n", strerror(errno));

    char buffer[1024];
    bool server_exit = false;
    while (!server_exit) {
        client_sock = accept(client_fd, (struct sockaddr*)&addr, &addrlen);
        CATCH(client_sock == -1, "Error accepting connection: %s\n", strerror(errno));
        puts("Received connection");

        bool connection_closed = false;
        while (!server_exit && !connection_closed) {
            memset(&buffer, 0, 1024);
            int bytes_read = read(client_sock, buffer, 1024);
            CATCH(bytes_read == -1, "Error reading from client socket: %s\n", strerror(errno));
            connection_closed = bytes_read == 0;
            if (connection_closed) {
                puts("Client closed connection");
                break;
            }

            server_exit = (strncasecmp(buffer, "/shutdown", 9) == 0);
            if (server_exit) {
                dprintf(client_sock, "Shutting down...\r\n");
                break;
            }
            dprintf(client_sock, "Echo: %s", buffer);
        }
        CATCH(close(client_sock) == -1, "Error closing client socket: %s\n", strerror(errno));
    }

    puts("Shutting down.");
    CATCH(close(client_fd) == -1, "Error closing server socket: %s\n", strerror(errno));
    
    return exit_status;
}
