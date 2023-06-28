#include "../my_macros.h"
#include <fcntl.h>
#include <fts.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHA1_FORMAT                                                            \
	"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"   \
	"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

#define SHA1_ARRAY(x)                                                          \
	x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9], x[10],     \
	 x[11], x[12], x[13], x[14], x[15], x[16], x[17], x[18], x[19]

struct file {
	char *path;
	long size;
	unsigned char has_checksum;
	unsigned char checksum[SHA_DIGEST_LENGTH];
	const char gap[27];
};

static int
compar(void *const a, void *const b)
{
	int memcmp_return;
	struct file *const a_file = (struct file *)a;
	int fd;
	struct file *const b_file = (struct file *)b;
	const void *data;

	if (a_file->size > b_file->size) {
		return 0;
	} else if (a_file->size < b_file->size) {
		return 1;
	}

	if (!a_file->has_checksum) {
		fd = open(a_file->path, 0);
		WARNIF(fd == -1);
		data = mmap(
		 0, (unsigned long)a_file->size, PROT_READ, MAP_PRIVATE, fd, 0);
		close(fd);
		SHA1(data, (unsigned long)a_file->size, a_file->checksum);
		a_file->has_checksum = 1;
	}

	if (!b_file->has_checksum) {
		fd = open(b_file->path, 0);
		WARNIF(fd == -1);
		data = mmap(
		 0, (unsigned long)b_file->size, PROT_READ, MAP_PRIVATE, fd, 0);
		close(fd);
		SHA1(data, (unsigned long)b_file->size, b_file->checksum);
		b_file->has_checksum = 1;
	}

	memcmp_return =
	 memcmp(a_file->checksum, b_file->checksum, SHA_DIGEST_LENGTH);
	if (memcmp_return > 0) {
		return 0;
	} else if (memcmp_return < 0) {
		return 1;
	} else {
		return 0;
	}
}

int
main()
{
	const FTSENT *node;
	unsigned int i = 0;
	char *const path_argv[] = {".", 0};
	struct file *files = malloc(1);
	unsigned char *previous_checksum[SHA_DIGEST_LENGTH] = {0};
	FTS *tree = fts_open(path_argv, 0, 0);

	WARNIF(!tree);
	for (node = fts_read(tree); node; node = fts_read(tree)) {
		if (node->fts_info != FTS_F) {
			continue;
		}

		files = realloc(files, (i + 1) * sizeof(struct file));

		files[i].path = malloc(node->fts_pathlen + 1);
		memcpy(files[i].path, node->fts_path, node->fts_pathlen + 1);

		files[i].has_checksum = 0;

		files[i].size = node->fts_statp->st_size;

		++i;
	}
	WARNIF(fts_close(tree));

	qsort(files, i, sizeof(struct file), compar);

	for (; i--; free(files[i].path)) {
		if (!files[i].has_checksum) {
			continue;
		}
		if (!memcmp(
		     files[i].checksum, previous_checksum, SHA_DIGEST_LENGTH)) {
			printf("\t%s\n", files[i].path);
		} else {
			memcpy(previous_checksum, files[i].checksum,
			 SHA_DIGEST_LENGTH);
			printf("\n" SHA1_FORMAT " | %ldB\n\t%s\n",
			 SHA1_ARRAY(files[i].checksum), files[i].size,
			 files[i].path);
		}
	}
	free(files);
}
