#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define RECOMMENDED_ALIGNMENT 16
#define GAP_SIZE 7
struct properties {
	char *d_name;
	unsigned char d_type;
	char gap[GAP_SIZE];
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static void check_file(char *file, char **include, char **exclude)
{
	int fd = open(file, O_CLOEXEC);
	if (fd == -1) {
		return;
	}
	unsigned long len = (unsigned long)lseek(fd, 0, SEEK_END);
	if (!len) {
		close(fd);
		return;
	}
	char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);

	for (; *include; ++include) {
		if (!strstr(data, *include)) {
			return;
		}
	}
	for (; *exclude; ++exclude) {
		if (strstr(data, *exclude)) {
			return;
		}
	}
	printf("%s\n", file);
}

int main(int argc, char **argv)
{
	char *directory = 0;
	char **include = calloc(1, sizeof(char *));
	char **exclude = calloc(1, sizeof(char *));
	for (int includes = 0, excludes = 0,
				opt = getopt(argc, argv, "d:i:x:");
			opt != -1;
			opt = getopt(argc, argv, "d:i:x:")) {
		if (opt == 'd') {
			directory = optarg;
		} else if (opt == 'i') {
			include[includes++] = optarg;
			include = realloc(include,
				sizeof(char *) * (includes + 1));
			include[includes] = 0;
		} else if (opt == 'x') {
			exclude[excludes++] = optarg;
			exclude = realloc(exclude,
				sizeof(char *) * (excludes + 1));
			exclude[includes] = 0;
		}
	}
	if (argv[optind] || !directory) {
		free(include);
		free(exclude);
		return 1;
	}

	unsigned int total_files = 1;
	struct properties *stack =
		malloc(total_files * sizeof(struct properties));

	/* Append the trailing slash if it does not exist. */
	asprintf(&stack->d_name, directory[strlen(directory) - 1] == '/' ?
		"%s" : "%s/", directory);

	stack->d_type = DT_DIR;
	struct dirent **namelist = 0;
	unsigned int i = 0;
	for (int j = 0; i < total_files; ++i) {
		if (stack[i].d_type == DT_REG) {
			check_file(stack[i].d_name, include, exclude);
		} else if (stack[i].d_type != DT_DIR) {
			continue;
		}
		j = scandir(stack[i].d_name, &namelist, 0, 0);
		if (j == -1) {
			/* Skip directories without read permission. */
			continue;
		}
		stack = realloc(stack,
			(total_files + (unsigned int)j - 2) *
				sizeof(struct properties));
		for (; j--; free(namelist[j])) {
			if (namelist[j]->d_name[0] == '.' && (
				(namelist[j]->d_name[1] == '.' &&
				namelist[j]->d_name[2] == 0 ) ||
				namelist[j]->d_name[1] == 0)
			) {
				continue;
			}
			stack[total_files].d_type =
				namelist[j]->d_type;
			asprintf(&stack[total_files].d_name,
				namelist[j]->d_type == DT_DIR ? "%s%s/" :
				"%s%s", stack[i].d_name, namelist[j]->d_name);
			++total_files;
		}
		free(namelist);
	}

	for (i = 0; i < total_files; ++i)  {
		free(stack[i].d_name);
	}
	free(stack);
	free(include);
	free(exclude);
}
