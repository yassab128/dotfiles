//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(char *program_name)
{
	printf("usage: %1$s [AMOUNT OF RAM TO OCCUPY IN MiB]\n"
		"example: `%1$s 50` (occupy 50 MiB until the program exits.)\n",
		program_name);
}

#define DECIMAL_BASE 10
static unsigned long calculate_bytes_to_allocate(char *str)
{
	char *prefix = 0;
	unsigned long size = (unsigned long)strtol(str, &prefix, DECIMAL_BASE);

	if (*(prefix + 1)) {
		return 0;
	}
	char supported_units[] = {'B', 'K', 'M', 'G'};
	for (char *ptr = supported_units; *ptr; ++ptr) {
		if (*ptr != *prefix) {
			continue;
		}
		unsigned long left_shift_value =
			(unsigned long) (ptr - supported_units) * DECIMAL_BASE;
		return size << left_shift_value;
	}
	return 0;
}

static void get_ram_usage()
{
	char line[80];
	FILE *file = fopen("/proc/self/statm", "re");
	fgets(line, sizeof line, file);
	fclose(file);
}

int main(int argc, char **argv)
{
	get_ram_usage();
	if (argc != 2) {
		usage(*argv);
		return 1;
	}
	++argv;

	unsigned long bytes_to_allocate = calculate_bytes_to_allocate(*argv);
	if (!bytes_to_allocate) {
		return 1;
	}

	void *memory = malloc(bytes_to_allocate);
	/*
	* The memory will not actually be filled unless the array elements
	* are initialized.
	*/
	memset(memory, 0, bytes_to_allocate);

	printf("%s (%luB) has been allocated. Press 'Enter' key to stop.\n",
		*argv, bytes_to_allocate);
	getchar();
	free(memory);
}
