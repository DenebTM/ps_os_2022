#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(/*int argc, char* argv[]*/) {
	/*if(argc != 2) {
	    fprintf(stderr, "Usage: %s [path]\n", argv[0]);
	    return EXIT_FAILURE;
	}*/

	// create pipe
	// [0] is read, [1] is write
	int pipefd[2];
	if(pipe(pipefd) == -1) {
		perror("Error creating pipe");
		return EXIT_FAILURE;
	}

	int cpid = fork();
	// fork failed
	if(cpid == -1) {
		perror("Error creating child");
		return EXIT_FAILURE;
	}
	// we are the parent - run wc -l
	if(cpid) {
		// close the write end of the pipe
		close(pipefd[1]);

		dup2(pipefd[0], STDIN_FILENO);
		execlp("wc", "wc", "-l", (char*)NULL);

		// close the read end of the pipe, wait for the child and exit
		close(pipefd[0]);
		int wstatus;
		wait(&wstatus);
		if(WIFSIGNALED(wstatus)) {
			fprintf(stderr, "Child was terminated by %s\n", strsignal(WTERMSIG(wstatus)));
		} else if(WEXITSTATUS(wstatus) != EXIT_SUCCESS) {
			fprintf(stderr, "Child exited with status %d\n", WEXITSTATUS(wstatus));
			return EXIT_FAILURE;
		}
		exit(EXIT_SUCCESS);
	}
	// we are the child - run ls
	else {
		close(pipefd[0]); // close the read end of the pipe
		dup2(pipefd[1], STDOUT_FILENO);
		execlp("ls", "ls", /*argv[1],*/ (char*)NULL);

		close(pipefd[1]);
		exit(EXIT_SUCCESS);
	}

	return EXIT_SUCCESS;
}
