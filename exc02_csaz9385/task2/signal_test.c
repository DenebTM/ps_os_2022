#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

static void handle_sig(int signum) {
    const char* str;
    int len = 0;

    switch (signum) {
        case SIGINT:
            // Triggered by Ctrl+C
            str = "Caught SIGINT\n";
            break;
        case SIGUSR1:
            str = "Caught SIGUSR1\n";
            break;
        case SIGTERM:
            // Triggered by kill [pid]
            str = "Caught SIGTERM\n";
            break;
        case SIGKILL:
            // Triggered by kill -9 [pid]
            // SIGKILL cannot actually be caught on the program side
            //   so kill -9 will always, invariably, exit
            str = "Caught SIGKILL\n";
            break;
    }

    len = strlen(str);
    write(STDOUT_FILENO, str, len);
}

int main() {
    struct sigaction act = {
        .sa_handler = &handle_sig,
        .sa_flags = SA_RESTART
    };
    sigfillset(&act.sa_mask);

    // Register signal handlers
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGKILL, &act, NULL);

    printf("Listening for signals\n");
    for(;;) {
        usleep(1000);
    }
    
    return EXIT_SUCCESS;
}
