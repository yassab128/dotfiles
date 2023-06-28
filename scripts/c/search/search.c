#include <assert.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

struct files {
	struct files *next;
	char *data;
	unsigned short data_length;
	char gap[14];
};

static void
print_and_free(struct files *head, struct files *tail)
{
	struct files *temp;

	putchar('\n');
	while (head != tail) {
		fwrite(head->data, 1, head->data_length, stdout);
		free(head->data);
		temp = head;
		head = head->next;
		free(temp);
	}
	free(tail);
}

int
main(const int argc, char *const *const argv)
{
	FTS *tree;
	const FTSENT *node;
	char *path_argv[2];
	struct files *directories_head;
	struct files *directories_tail;
	struct files *regular_files_head;
	struct files *regular_files_tail;

	if (argc != 3) {
		fputs("argc != 2\n", stderr);
		return 1;
	}
	path_argv[0] = argv[1];
	path_argv[1] = (char *)0;

	tree = fts_open(path_argv, 0, 0);
	assert(tree);

	directories_head = malloc(sizeof(struct files));
	assert(directories_head);
	directories_tail = directories_head;
	regular_files_head = malloc(sizeof(struct files));
	assert(regular_files_head);
	regular_files_tail = regular_files_head;
	for (node = fts_read(tree); node; node = fts_read(tree)) {
		switch (node->fts_info) {
		case FTS_DP:
			continue;
		default:
			break;
		}
		if (!strcasestr(node->fts_name, argv[2])) {
			continue;
		}
		switch (node->fts_info) {
		case FTS_D:
			directories_tail->data_length = node->fts_pathlen + 2;
			directories_tail->data =
			 malloc(directories_tail->data_length);
			assert(directories_tail->data);
			memcpy(directories_tail->data, node->fts_path,
			 node->fts_pathlen);
			directories_tail->data[node->fts_pathlen] = '/';
			directories_tail->data[node->fts_pathlen + 1] = '\n';
			directories_tail->next = malloc(sizeof(struct files));
			assert(directories_tail->next);
			directories_tail = directories_tail->next;
			break;
		default:
			regular_files_tail->data_length = node->fts_pathlen + 2;
			regular_files_tail->data =
			 malloc(regular_files_tail->data_length);
			assert(regular_files_tail->data);
			memcpy(regular_files_tail->data, node->fts_path,
			 node->fts_pathlen);
			switch (node->fts_statp->st_mode & S_IFMT) {
			case S_IFIFO:
				regular_files_tail->data[node->fts_pathlen] =
				 '|';
				break;
			case S_IFLNK:
				regular_files_tail->data[node->fts_pathlen] =
				 '@';
				break;
			case S_IFSOCK:
				regular_files_tail->data[node->fts_pathlen] =
				 '=';
				break;
			default:
				/*
				 * https://github.com/
				 * openbsd/src/blob/master/bin/ls/print.c
				 */
				if (node->fts_statp->st_mode &
				    (S_IXUSR | S_IXGRP | S_IXOTH)) {
					regular_files_tail
					 ->data[node->fts_pathlen] = '*';
				} else {
					regular_files_tail
					 ->data[node->fts_pathlen] = ' ';
				}
				break;
			}
			regular_files_tail->data[node->fts_pathlen + 1] = '\n';
			regular_files_tail->next = malloc(sizeof(struct files));
			assert(regular_files_tail->next);
			regular_files_tail = regular_files_tail->next;
			break;
		}
	}
	print_and_free(directories_head, directories_tail);
	print_and_free(regular_files_head, regular_files_tail);
	assert(!fts_close(tree));
}
