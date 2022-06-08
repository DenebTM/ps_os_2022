#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

enum childpid { CHILD_LS, CHILD_GREP };

int main(/*int argc, char* argv[]*/) {
	/*if(argc != 2) {
	    fprintf(stderr, "Usage: %s [path]\n", argv[0]);
	    return EXIT_FAILURE;
	}*/

	// create two pipes
	//  pipefd_lsgrep for the pipe between ls and grep
	//  pipefd_grepwc for the pipe between grep and wc
	// [0] is read, [1] is write
	int pipefd_lsgrep[2], pipefd_grepwc[2];
	if(pipe(pipefd_lsgrep) == -1 || pipe(pipefd_grepwc) == -1) {
		perror("Error creating pipe");
		return EXIT_FAILURE;
	}

	int cnum;
	// create two children
	//  [0] for ls
	//  [1] for grep
	for(cnum = 0; cnum < 2; cnum++) {
		pid_t cpid = fork();
		// fork failed
		if(cpid == -1) {
			perror("Error creating child");
			return EXIT_FAILURE;
		}
		// we are a child, run ls or grep
		else if(cpid == 0) {
			if(cnum == CHILD_LS) {
				// close unused pipes
				close(pipefd_lsgrep[0]);

				close(pipefd_grepwc[0]);
				close(pipefd_grepwc[1]);

				// replace stdout with the write end of pipefd_lsgrep
				dup2(pipefd_lsgrep[1], STDOUT_FILENO);
				execlp("ls", "ls", /*argv[1],*/ (char*)NULL);

				// close remaining pipe and exit
				close(pipefd_lsgrep[1]);
				exit(EXIT_SUCCESS);
			} else if(cnum == CHILD_GREP) {
				// close unused pipes
				close(pipefd_lsgrep[1]);
				close(pipefd_grepwc[0]);

				// replace stdin with the read end of pipefd_lsgrep
				//   and stdout with the write end of pipefd_grepwc
				dup2(pipefd_lsgrep[0], STDIN_FILENO);
				dup2(pipefd_grepwc[1], STDOUT_FILENO);
				execlp("grep", "grep", "-v", "lab", (char*)NULL);

				// close remaining pipes and exit
				close(pipefd_lsgrep[0]);
				close(pipefd_grepwc[1]);
				exit(EXIT_SUCCESS);
			}
			// this should not be reached
			else {
				fputs("?????\n", stderr);
				exit(EXIT_FAILURE);
			}
		}
	}

	// we are the parent - run wc -l
	// close unused pipes
	close(pipefd_lsgrep[0]);
	close(pipefd_lsgrep[1]);

	close(pipefd_grepwc[1]);

	// replace stdin with the read end of pipefd_grepwc
	dup2(pipefd_grepwc[0], STDIN_FILENO);
	execlp("wc", "wc", "-l", (char*)NULL);

	// close remaining pipe, wait for children and exit
	close(pipefd_grepwc[0]);
	int wstatus;
	for(int i = 0; i < 2; i++) {
		wait(&wstatus);
		if(WIFSIGNALED(wstatus)) {
			fprintf(stderr, "Child %d was terminated by %s\n", i, strsignal(WTERMSIG(wstatus)));
			return EXIT_FAILURE;
		} else if(WEXITSTATUS(wstatus) != EXIT_SUCCESS) {
			fprintf(stderr, "Child %d exited with status %d\n", i, WEXITSTATUS(wstatus));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}
