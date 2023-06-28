//true; exec ccompile "$0" -- "$@"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Provide a file-path as an argument.\n");
		return 1;
	}
	char *line = 0;
	unsigned long len = 0;
	FILE *fp = fopen(*(argv + 1), "re");

	/* Check whether fp points to a valid path */
	if (!fp) {
		printf("Something is wrong with the provided path.\n");
		return 1;
	}
	long read = 0;
	read = getline(&line, &len, fp);

	/* Check whether fp points to an a directory or an empty file. */
	if (read == -1) {
		printf("The file is not valid.\n");
		free(line);
		return 1;
	}
	write(1, line, (unsigned long)read);

	for (; read != -1; read = getline(&line, &len, fp)) {
		write(1, line, (unsigned long)read);
	}
	fclose(fp);
	free(line);
}
