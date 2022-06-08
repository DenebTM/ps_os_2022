#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PREFIX "/tmp/csaz9385_exc04_"
#define FIFO_MAXLEN 40

char** client_names;
char** fifo_names;
struct pollfd* fifo_pollfds;
int num_clients, num_successful, num_connections;
char buf[PIPE_BUF];

// tries to create a named pipe for each client specified and open it
int create_client_fifos(char** const names, const int num_clients) {
	for(int i = 0; i < num_clients; i++) {
		num_successful = i;
		snprintf(fifo_names[i], FIFO_MAXLEN, FIFO_PREFIX "%s", names[i]);
        // avoid a failure case where mkfifo cannot create a file that already
        // exists after an unclean exit; failures do not need to be caught
        unlink(fifo_names[i]);
		if(mkfifo(fifo_names[i], 0600) == -1) {
			fprintf(stderr, "Failed to create FIFO at %s\n", fifo_names[i]);
			return num_successful;
		}
		int fd = open(fifo_names[i], O_RDONLY);
		if(fd == -1) {
			fprintf(stderr, "Failed to open FIFO at %s\n", fifo_names[i]);
			return num_successful;
		}
		fifo_pollfds[i].fd = fd;
		fifo_pollfds[i].events = POLLIN | POLLERR | POLLHUP;
		fifo_pollfds[i].revents = 0;
		printf("%s connected\n", client_names[i]);
		num_connections++;
	}

	return num_successful = num_clients;
}

void delete_client_fifos(const int num_clients) {
	for(int i = 0; i < num_clients; i++) {
		// close will fail silently if fd is already closed, that's fine
		close(fifo_pollfds[i].fd);
		unlink(fifo_names[i]);
	}
}

// remove all created pipes, free allocated memory and exit
void exit_cleanup_status(int status) {
	printf("Exiting...\n");
	delete_client_fifos(num_successful);
	for(int i = 0; i < num_successful; i++) {
		free(fifo_names[i]);
	}
	free(fifo_names);
	free(fifo_pollfds);

	exit(status);
}
void exit_cleanup() {
	exit_cleanup_status(EXIT_SUCCESS);
}

// remove whitespace characters from the buffer to simplify pattern matching
void rem_whitespace(char* str) {
	int in_ptr = 0, out_ptr = 0;
	for(; str[in_ptr]; in_ptr++) {
		if(str[in_ptr] == ' ' || str[in_ptr] == '\t' || str[in_ptr] == '\n' ||
		   str[in_ptr] == '\r') {
			continue;
		}
		str[out_ptr++] = str[in_ptr];
	}
	str[out_ptr] = '\0';
}

// tries to parse an expression in expr, writes the result into result, and
// returns either 0 (success) or -1 (error)
int parse_expr(double* result, char* expr) {
	double num1, num2;
	int offset;

	// try to match the string to x+y, x-y, x*y or x/y
	// if all of these fail, return with an error
	if(sscanf(expr, "%lf+%lf%n", &num1, &num2, &offset) == 2) {
		*result = num1 + num2;
	} else if(sscanf(expr, "%lf-%lf%n", &num1, &num2, &offset) == 2) {
		*result = num1 - num2;
	} else if(sscanf(expr, "%lf*%lf%n", &num1, &num2, &offset) == 2) {
		*result = num1 * num2;
	} else if(sscanf(expr, "%lf/%lf%n", &num1, &num2, &offset) == 2) {
		*result = num1 / num2;
	} else {
		return -1;
	}

	// catch spurious characters after the expression
	if(buf[offset] != '\0') {
		return -1;
	}
	return 0;
}

int main(int argc, char** const argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s [client name] [client names...]\n", argv[0]);
		return EXIT_FAILURE;
	}

	// allocate the necessary buffers for each client name passed via argv
	num_clients = argc - 1;
	client_names = &argv[1];
	fifo_names = calloc(num_clients, sizeof(char*));
	for(int i = 0; i < num_clients; i++) {
		fifo_names[i] = malloc(sizeof(char) * FIFO_MAXLEN);
	}
	fifo_pollfds = malloc(num_clients * sizeof(struct pollfd));

	// catch 'kill' and Ctrl+C to run clean-up tasks before exiting
	signal(SIGTERM, &exit_cleanup);
	signal(SIGINT, &exit_cleanup);

	// wait for clients - if an error occurs, clean up and exit
	puts("Waiting for clients...");
	create_client_fifos(client_names, num_clients);
	if(num_successful != num_clients) {
		exit_cleanup(EXIT_FAILURE);
	}

	puts("All clients are now connected, awaiting expressions.");
	while(num_connections) {
		// check for input on each client pipe
		if(poll(fifo_pollfds, num_clients, 0)) {
			for(int i = 0; i < num_clients; i++) {
				switch(fifo_pollfds[i].revents) {
					// input available
					case POLLIN:
						read(fifo_pollfds[i].fd, buf, PIPE_BUF);
						if(buf[0] != '\n') {
							rem_whitespace(buf);
							double result;
							if(parse_expr(&result, buf) == -1) {
								printf("%s: %s is malformed\n", client_names[i], buf);
							} else {
								printf("%s: %s = %lf\n", client_names[i], buf, result);
							}
						}
						break;
					// client closed the pipe
					case POLLHUP:
						printf("%s disconnected\n", client_names[i]);
						close(fifo_pollfds[i].fd);
						num_connections--;
						break;
					// error
					case POLLERR:
						fputs("An error occurred\n", stderr);
						num_connections = 0;
						break;
				}
				fifo_pollfds[i].revents = 0;
			}
		}
		usleep(10000);
	}

	exit_cleanup();
}
