#include "../my_macros.h"
#include <fcntl.h>
#include <fts.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXTHREADS 131072

/* 126914 */

static void *
just_sleep(void *arg)
{
	(void)arg;
	puts("heil");
	return EXIT_SUCCESS;
}

static void
testing_threads()
{
	pthread_t thread_id[MAXTHREADS];
	unsigned int i = 0;
	unsigned int n = 0;

	for (; i < MAXTHREADS; ++i) {
		CHECK(pthread_create(thread_id + i, 0, just_sleep, (void *)0));
		printf("%d\n", i);
	}

	for (; n < i; n++) {
		CHECK(pthread_join(thread_id[i], 0));
	}
}

static int
compare(const FTSENT **one, const FTSENT **two)
{
	return ((*one)->fts_namelen > (*two)->fts_namelen);
}

struct human_size {
	double value;
	char unit;
	char pad[7];
};

static struct human_size
convert_size(long size)
{
	struct human_size file_size;
	if (size > 1073741824) {
		file_size.value = (double)size / 1073741824;
		file_size.unit = 'G';
	} else if (size > 1048576) {
		file_size.value = (double)size / 1048576;
		file_size.unit = 'M';
	} else if (size > 1024) {
		file_size.value = (double)size / 1024;
		file_size.unit = 'K';
	} else {
		file_size.value = (double)size;
		file_size.unit = 'B';
	}
	return file_size;
}

#define MAX_LINE_LENGTH 32767

int
main(const int argc, const char *const *const argv)
{
	char *const path_argv[] = {"/", 0};
	FTS *const tree = fts_open(path_argv, 0, &compare);
	struct human_size file_size;
	const FTSENT *node;
	int pipefd[2];
	unsigned long files_list_size = 0;
	unsigned long lines = 0;
	char *files_list_buffer;
	FILE *fp;

	(void)argv;
	(void)argc;

	/* 12884 */
#if 0
	CHECK(pipe(pipefd));
#else
	CHECK(pipe2(pipefd, O_NONBLOCK));
#endif

	fp = fdopen(pipefd[1], "w");
	CHECK(!fp);
	for (node = fts_read(tree); node; node = fts_read(tree)) {
		++lines;
		file_size = convert_size(node->fts_statp->st_size);
		files_list_size +=
		 (unsigned long)fprintf(fp, "%s %.1f%c %ld\n", node->fts_path,
		  file_size.value, file_size.unit, node->fts_statp->st_size);
	}
	CHECK(close(pipefd[1]));
	CHECK(fts_close(tree));

	files_list_buffer = malloc(files_list_size);
	if (files_list_buffer) {
		CHECK(
		 read(pipefd[0], files_list_buffer, files_list_size) == -1);
		CHECK(close(pipefd[0]));
		printf("\n%lu | %lu %lu\n", lines, files_list_size,
		 fwrite(files_list_buffer, 1, files_list_size, stdout));
		free(files_list_buffer);
	} else {
		char line[MAX_LINE_LENGTH];
		fp = fdopen(pipefd[0], "r");
		while (fgets(line, MAX_LINE_LENGTH, fp)) {
			fputs(line, stdout);
		}
		CHECK(fclose(fp));
	}

	return EXIT_SUCCESS;
}
