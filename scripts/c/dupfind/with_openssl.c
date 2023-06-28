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

struct file {
	char *path;
	long size;
	unsigned long dunno;
	unsigned char has_checksum;
	unsigned char checksum[SHA_DIGEST_LENGTH];
	char gap[19];
};

static int
compar(void *a, void *b)
{
	int memcmp_return;
	struct file *const a_file = (struct file *)a;
	int a_fd;
	const void *a_data;
	struct file *const b_file = (struct file *)b;
	int b_fd;
	const void *b_data;

#if 0
	if (a_file->size > b_file->size) {
		return 1;
	}
	if (a_file->size < b_file->size) {
		return -1;
	}
#else
	if (a_file->size != b_file->size) {
		return 0;
	}
#endif

	if (!a_file->has_checksum) {
		a_fd = open(a_file->path, 0);
		WARNIF(a_fd == -1);
		a_data = mmap(0, (unsigned long)a_file->size, PROT_READ,
		 MAP_PRIVATE, a_fd, 0);
		close(a_fd);
		SHA1(a_data, (unsigned long)a_file->size - 1, a_file->checksum);
		a_file->has_checksum = 1;
	}

	if (!b_file->has_checksum) {
		b_fd = open(b_file->path, 0);
		WARNIF(b_fd == -1);
		b_data = mmap(0, (unsigned long)b_file->size, PROT_READ,
		 MAP_PRIVATE, b_fd, 0);
		close(b_fd);
		SHA1(b_data, (unsigned long)b_file->size - 1, b_file->checksum);
		b_file->has_checksum = 1;
	}

	memcmp_return =
	 memcmp(a_file->checksum, b_file->checksum, SHA_DIGEST_LENGTH);
	if (!memcmp_return) {
		++a_file->dunno;
		++b_file->dunno;
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
	unsigned int j = 0;
	char *const path_argv[] = {".", 0};
	struct file *files = malloc(1);
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
		files[i].dunno = 0;

		files[i].size = node->fts_statp->st_size;

		++i;
	}
	WARNIF(fts_close(tree));

	qsort(files, i, sizeof(struct file), compar);

	for (; j < i; ++j) {
		if (1 || files[j].dunno) {
			printf("%s | %ld | %ld\n", files[j].path, files[j].size,
			 files[j].dunno);
		}
		free(files[j].path);
	}
	free(files);
}
