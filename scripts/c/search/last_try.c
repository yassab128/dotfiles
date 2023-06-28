#include <assert.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

static int
compar(const FTSENT **a, const FTSENT **b)
{
	if ((*a)->fts_statp->st_atim.tv_sec > (*b)->fts_statp->st_atim.tv_sec) {
		return 1;
	}
	if ((*a)->fts_statp->st_atim.tv_sec < (*b)->fts_statp->st_atim.tv_sec) {
		return -1;
	}
	return 0;
}

int
main(const int argc, char *const *const argv)
{
	FTS *tree;
	const FTSENT *node;
	char *path_argv[2];

	path_argv[0] = argv[1];
	path_argv[1] = (char *)0;

	tree = fts_open(path_argv, 0, &compar);
	assert(tree);

	for (node = fts_read(tree); node; node = fts_read(tree)) {
		switch (node->fts_info) {
		case FTS_DP:
			continue;
		default:
			break;
		}
		printf("%ld\n", node->fts_statp->st_atim.tv_sec);
	}
	assert(!fts_close(tree));
}
