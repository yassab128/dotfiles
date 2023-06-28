#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Take a look at:
 * Gootgle 'pipe dup2 fork read and write'
 * https://stackoverflow.com/a/7935257/12493245
 * https://stackoverflow.com/a/3910433/12493245
 */

#define TEST_STR "hello world!"
#define TEST_FILE "test_file"

static void read_write()
{
	int pipe_to_child[2];
	pipe2(pipe_to_child, O_CLOEXEC);
	int pipe_from_child[2];
	pipe2(pipe_from_child, O_CLOEXEC);
	pid_t pid = fork();
	if (pid == 0) {
		close(pipe_from_child[0]);
		close(pipe_to_child[1]);
		dup2(pipe_to_child[0], 0);
		close(pipe_to_child[0]);
		dup2(pipe_from_child[1], 1);
		close(pipe_from_child[1]);
		execl("/usr/bin/awk", "awk", "{print toupper($0)}", 0);
		_exit(0);
	}
	close(pipe_to_child[0]);
	close(pipe_from_child[1]);
	FILE *inputfile = fdopen(pipe_to_child[1], "w");
	fprintf(inputfile, TEST_STR);
	fclose(inputfile);
	close(pipe_to_child[1]);
	inputfile = fdopen(pipe_from_child[0], "r");
	for (int c = fgetc(inputfile); c != EOF; c = fgetc(inputfile)) {
		printf("%c", c);
	}
	fclose(inputfile);
	close(pipe_from_child[0]);
	waitpid(pid, 0, 0);
}

static void write_only()
{
	unlink(TEST_FILE);
	int pipe_to_child[2];
	pipe2(pipe_to_child, O_CLOEXEC);
	pid_t pid = fork();
	if (pid == 0) {
		close(pipe_to_child[1]);
		dup2(pipe_to_child[0], 0);
		close(pipe_to_child[0]);
		execl("/usr/bin/awk", "awk",
			"{print toupper($0)>\"" TEST_FILE "\"}", 0);
		_exit(0);
	}
	close(pipe_to_child[0]);
	FILE *inputfile = fdopen(pipe_to_child[1], "w");
	fprintf(inputfile, TEST_STR);
	fclose(inputfile);
	close(pipe_to_child[1]);
	waitpid(pid, 0, 0);

	inputfile = fopen(TEST_FILE, "re");
	for (int c = fgetc(inputfile); c != EOF; c = fgetc(inputfile)) {
		printf("%c", c);
	}
	fclose(inputfile);
	unlink(TEST_FILE);
}

static void read_only()
{
	int pipe_from_child[2];
	pipe2(pipe_from_child, O_CLOEXEC);
	pid_t pid = fork();
	if (pid == 0) {
		close(pipe_from_child[0]);
		dup2(pipe_from_child[1], 1);
		close(pipe_from_child[1]);
		execl("/usr/bin/awk", "awk",
			"BEGIN{print toupper(\"" TEST_STR "\")}", 0);
		_exit(0);
	}
	close(pipe_from_child[1]);
	FILE *inputfile = fdopen(pipe_from_child[0], "r");
	for (int c = fgetc(inputfile); c != EOF; c = fgetc(inputfile)) {
		printf("%c", c);
	}
	fclose(inputfile);
	close(pipe_from_child[0]);
	waitpid(pid, 0, 0);
}

int main()
{
	/* read_write(); */
	/* write_only(); */
	read_only();
}
