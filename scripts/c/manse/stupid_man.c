#include "../my_macros.h"
#include <dirent.h>
#include <fts.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define IS_NOT_A_GZIP_FILE(x, y)                                               \
	((x)[(y)-1] != 'z' || (x)[(y)-3] != '.' || (x)[(y)-2] != 'g')

/*
 * I think 4 MB is enough since the biggest man page is that of ffmpeg-all.1
 * which is ~2 MB.
 */
#define EIGHT_MB 8388608

#define MAX_ARGV_SIZE 255
static unsigned char patterns_number = 0;
static unsigned long needle_length[MAX_ARGV_SIZE];
static char *needle[MAX_ARGV_SIZE];

static void *
check_subdirectories(void *arg)
{
	struct dirent *const namelist = (struct dirent *)arg;
	char *path_argv[2];

	FTS *tree;
	FTSENT *node;
	char buffer[EIGHT_MB];
	gzFile file;
	int buffer_size;
	unsigned char i;

	path_argv[0] = namelist->d_name;
	free(arg);
	path_argv[1] = 0;
	tree = fts_open(path_argv, FTS_NOSTAT | FTS_NOCHDIR, 0);

	for (node = fts_read(tree); node; node = fts_read(tree)) {
		if (node->fts_info != FTS_F ||
		 IS_NOT_A_GZIP_FILE(node->fts_name, node->fts_namelen)) {
			continue;
		}
		file = gzopen(node->fts_path, "r");
		/*
		 * gzread() adds the null terminator, unless buffer_size is
		 * equal to EIGHT_MB.
		 */
		buffer_size = gzread(file, buffer, EIGHT_MB);
		for (i = 0; i < patterns_number; ++i) {
			if (!memmem(buffer, (unsigned long)buffer_size,
			     needle[i], needle_length[i])) {
				goto skip;
			}
		}
		puts(node->fts_path);
	skip:
		CHECK(gzclose_r(file));
	}
	CHECK(fts_close(tree));
	return 0;
}

/*
 * Threads will get created based on the number of sub-directories (man0p, man1,
 * etc)
 */

#define THREADS_MAX 64

#define NINE_MB 9437184

static void
check_directory(const char *man_path)
{
	int n;
	pthread_t thread_id[THREADS_MAX];
	pthread_attr_t attr;
	unsigned char i = 0;
	struct dirent **namelist;

	if (chdir(man_path)) {
		fputs("\nMANPATH contains a non-existent path.\n", stderr);
		return;
	}

	CHECK(pthread_attr_init(&attr));
	/*
	 * Set the default stack size per thread to 8 MB (the default stack size
	 * in the main thread on Linux) instead of the default 2 MB.
	 */
	CHECK(pthread_attr_setstacksize(&attr, NINE_MB));

	for (n = scandir(".", &namelist, 0, 0); n--;) {
		if (namelist[n]->d_type != DT_DIR ||
		 namelist[n]->d_name[0] == '.') {
			free(namelist[n]);
			continue;
		}
		CHECK(pthread_create(thread_id + i, &attr, check_subdirectories,
		 (void *)namelist[n]));
		++i;
	}
	CHECK(pthread_attr_destroy(&attr));
	for (n = 0; n < i; ++n) {
		CHECK(pthread_join(thread_id[n], 0));
	}
	free(namelist);
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [FIXED_STRING]\n", argv[0]);
		return 1;
	}
	for (; argv[patterns_number + 1]; ++patterns_number) {
		needle[patterns_number] = argv[patterns_number + 1];
		needle_length[patterns_number] =
		 strlen(needle[patterns_number]);
	}
#if 0
	char *man_paths = getenv("MANPATH");
	CHECK(!man_paths);
	for (char *man_path = strtok(man_paths, ":"); man_path;
	     man_path = strtok(0, ":")) {
		check_directory(man_path);
	}
#else
	check_directory("/usr/share/man");
#endif
	return EXIT_SUCCESS;
}
