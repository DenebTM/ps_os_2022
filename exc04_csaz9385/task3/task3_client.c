#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PREFIX "/tmp/csaz9385_exc04_"
#define FIFO_MAXLEN 40

int client_fd;
char buf[PIPE_BUF];

// free allocated memory and exit
void exit_cleanup() {
	printf("Exiting...\n");
	close(client_fd);

	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s [client name]\n", argv[0]);
		return EXIT_FAILURE;
	}

	// find and open the correct pipe in write mode
	char fname[FIFO_MAXLEN];
	snprintf(fname, FIFO_MAXLEN, FIFO_PREFIX "%s", argv[1]);
	client_fd = open(fname, O_WRONLY);
	if(client_fd == -1) {
		fprintf(stderr, "Failed to open FIFO %s\n", fname);
		return EXIT_FAILURE;
	}

	printf("Connected\n");

	// catch 'kill' and Ctrl+C to run clean-up tasks before exiting
	signal(SIGTERM, &exit_cleanup);
	signal(SIGINT, &exit_cleanup);

	// send input to the server until an empty line is read
	buf[0] = '\0';
	ssize_t num_bytes;
	while(buf[0] != '\n') {
		num_bytes = read(STDIN_FILENO, buf, PIPE_BUF);
		buf[num_bytes] = '\0';
		write(client_fd, buf, num_bytes + 1);
		usleep(10000);
	}
	exit_cleanup();
}
