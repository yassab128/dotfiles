#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto -- "$@"
#endif

/* FIXME: Clang finds memory problems with this program. */

#include <dirent.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <preprocessor_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define RECOMMENDED_ALIGNMENT 64
#define GAP_SIZE 26
struct properties {
	char *d_name;
	unsigned long size;
	unsigned char checksum[SHA_DIGEST_LENGTH];
	char gap[GAP_SIZE];
	unsigned char has_been_processed;
	unsigned char d_type;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static void get_checksum(struct properties *stack)
{
	int fd = open(stack->d_name, O_CLOEXEC);
	if (fd == -1) {
		return;
	}
	stack->size = (unsigned long)lseek(fd, 0, SEEK_END);
	void *data = mmap(0, stack->size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	SHA1(data, stack->size, stack->checksum);
}

static void get_files(char *directory)
{
	/* Changing 2 to 1 causes problems. */
	struct properties *stack = malloc(2 * sizeof(struct properties));

	/* Append the trailing slash if it does not exist. */
	asprintf(&stack->d_name, directory[strlen(directory) - 1] == '/' ?
		"%s" : "%s/", directory);

	stack->d_type = DT_DIR;
	struct dirent **namelist = 0;
	unsigned int i = 0;
	unsigned int total_files = 1;
	for (int j = 0; i < total_files; ++i) {
		stack[i].has_been_processed = 0;
		if (stack[i].d_type == DT_REG) {
			get_checksum(&stack[i]);
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
	free(stack[0].d_name);

	for (i = 1; i < total_files; ++i)  {
		if (stack[i].d_type != DT_REG || stack[i].has_been_processed) {
			free(stack[i].d_name);
			continue;
		}
		for (unsigned int j = i + 1; j < total_files; ++j)  {
			if (!stack[j].has_been_processed &&
				!memcmp(stack[i].checksum, stack[j].checksum,
					SHA_DIGEST_LENGTH))
			{
				stack[j].has_been_processed = 1;
				stack[i].has_been_processed = 1;
				puts(stack[j].d_name);
			}
		}
		if (stack[i].has_been_processed) {
			unsigned char unit = 0;
			printf("%s -> %.2f%c\n\n", stack[i].d_name,
				HUMAN_SIZE((double)stack[i].size, unit));
		}
		free(stack[i].d_name);
	}
	free(stack);
}

int main()
{
	/* get_files("/home/aptrug/Programme/"); */
	get_files("/home");
}
