#include "../my_macros.h"
#include <fcntl.h>
#include <fts.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

struct file {
	char *path;
	char *hash;
	long size;
	char gap[8];
};

int
main()
{
	const FTSENT *node;
	unsigned int i = 0;
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

		files[i].size = node->fts_statp->st_size;

		++i;
	}
	WARNIF(fts_close(tree));

	while (i--) {
		puts(files[i].path);
		free(files[i].path);
	}
	free(files);
}
