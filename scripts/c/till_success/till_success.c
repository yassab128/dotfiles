#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	int max_attempts = atoi(argv[1]);
	if (!max_attempts) {
		fprintf(stderr, "Usage: %s [MAX_ATTEMPTS_NUMBER] [COMMAND]\n",
		 argv[0]);
		return EXIT_FAILURE;
	}
	for (int pid, exit_code = 1; exit_code; --max_attempts) {
		pid = fork();
		if (!pid) {
			execvp(argv[2], argv + 2);
		}
		waitpid(pid, &exit_code, 0);
		if (!max_attempts) {
			puts("Too many failed attempts!");
			return EXIT_FAILURE;
		}
	}
}
