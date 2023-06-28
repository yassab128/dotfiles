#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BILLION 1000000000

int
main(int argc, char **argv)
{
	struct timespec begin;
	struct timespec end;
	int rerun_for;
	int retval;
	int pid;

	if (argc < 3) {
		return EXIT_FAILURE;
	}
	rerun_for = atoi(argv[1]);

	if (!rerun_for) {
		return EXIT_FAILURE;
	}
#if 0
	// struct timeval start = {0};
	// gettimeofday(&start, 0);
#endif
	retval = clock_gettime(CLOCK_MONOTONIC, &begin);
	assert(retval != -1);
	while (rerun_for--) {
		putchar('\n');
		pid = fork();
		switch (pid) {
		case 0:
			execvp(argv[2], argv + 2);
			_exit(1);
		case -1:
			perror("fork()");
			_exit(1);
		default:
			break;
		}
		waitpid(pid, 0, 0);
	}
#if 0
	struct timeval stop = {0};
	gettimeofday(&stop, 0);
	printf("Total elapsed time %ld.%ld\n", end.tv_sec - begin.tv_sec,
	end.tv_nsec - begin.tv_nsec);
#endif

	retval = clock_gettime(CLOCK_MONOTONIC, &end);
	assert(retval != -1);
	if (end.tv_nsec < begin.tv_nsec) {
		printf("Total elapsed time %ld.%09ld\n",
		 end.tv_sec - begin.tv_sec - 1,
		 BILLION + end.tv_nsec - begin.tv_nsec);
	} else {
		printf("Total elapsed time %ld.%09ld\n",
		 end.tv_sec - begin.tv_sec, end.tv_nsec - begin.tv_nsec);
	}
}
