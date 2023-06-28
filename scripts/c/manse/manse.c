#include <assert.h>
#include <dirent.h>
#include <fts.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <zlib.h>

#include <string.h>

#define MAX_PATH 255

#define MAX_THREADS 32768

/* LINUX 8380384 */
/* OPENBSD 519839 */
#define BUFSZ 262144

static int g_argc;
static char **g_argv;
static size_t g_argvlen[255];

void *
     memmem(const void *, size_t, const void *, size_t);

static void *
search_manpage(void *file_path)
{
	char buf[BUFSZ];
	gzFile file;
	int retval;
	int bytes_read;
	int argc = g_argc;

	file = gzopen(file_path, "r");
	puts(file_path);
	assert(file != NULL);

	for (;;) {
		bytes_read = gzread(file, buf, BUFSZ);
		if (bytes_read == -1) {
			break;
		}
		while (argc--) {
			if (memmem(buf, (size_t) bytes_read,
				   g_argv[argc], g_argvlen[argc]) == NULL) {
				continue;
			}
			retval = gzclose_r(file);
			assert(retval == Z_OK);
			return EXIT_SUCCESS;
		}
		if (bytes_read != BUFSZ) {
			break;
		}
	}
	retval = gzclose_r(file);
	assert(retval == Z_OK);
	return EXIT_SUCCESS;
}

static void
fill_argvlen(int argc)
{
	while (argc--) {
		g_argvlen[argc] = strlen(g_argv[argc]);
	}
}

int
main(int argc, char **argv)
{
	int retval;
	pid_t pid;
	int pipefd[2];
	char line[MAX_PATH];
	pthread_t thread_id[MAX_THREADS];
	FILE *fp;
	unsigned short i = 0;

	if (argc > 255 || argc < 2) {
		return 1;
	}
	g_argc = argc;
	g_argv = argv;
	fill_argvlen(argc);

	retval = pipe(pipefd);
	assert(retval == 0);

	pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
		_exit(1);
	case 0:
		retval = close(pipefd[0]);
		assert(retval == 0);

		retval = dup2(pipefd[1], 1);
		assert(retval != -1);

		retval = close(pipefd[1]);
		assert(retval == 0);

		execlp("man", "man", "-kw", ".", NULL);
		_exit(0);
	default:
		break;
	}
	retval = close(pipefd[1]);
	assert(retval == 0);

	fp = fdopen(pipefd[0], "r");
	assert(fp != NULL);

	while (fgets(line, MAX_PATH, fp)) {
		retval = pthread_create(
			thread_id + i, NULL, search_manpage, (void *) line);
		if (retval) {
			while (i--) {
				retval = pthread_join(thread_id[i], 0);
				assert(retval == 0);
			}
			i = 0;
			retval = pthread_create(
			thread_id + i, NULL, search_manpage, (void *) line);
			assert(retval == 0);
		}
		assert(retval == 0);

		++i;
	}

	while (i--) {
		retval = pthread_join(thread_id[i], 0);
		assert(retval == 0);
	}

	retval = fclose(fp);
	assert(retval == 0);

	retval = waitpid(pid, 0, 0);
	assert(retval != -1);
}
