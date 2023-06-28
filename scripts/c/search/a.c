#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_FWRITE 1

struct string {
	char *data;
	unsigned long length;
};

static void
print_array_and_free(struct string *array, unsigned short i)
{
	unsigned short j = 0;
	while (j < i) {
#if USE_FWRITE
#	if 1
		/* fwrite(array[j].data, 1, array[j].length, stdout); */
#	else
		write(1, array[j].data, array[j].length, 1);
#	endif
#else
		fputs(array[j].data, stdout);
#endif
		free(array[j].data);
		++j;
	}
}

static void
read_file(const char *const file)
{
	FILE *fp;
	char *lineptr = 0;
	long nread;
	unsigned long len;
	struct string *array;
	unsigned short i = 0;

	fp = fopen(file, "r");
	assert(fp);
	array = malloc(1);
	for (nread = getline(&lineptr, &len, fp); nread != -1;
	     nread = getline(&lineptr, &len, fp), ++i) {
		array = realloc(array, (i + 1) * sizeof(struct string));
		array[i].data = lineptr;
		array[i].length = (unsigned long)nread;
		lineptr = 0;
	}
	free(lineptr);
	fclose(fp);
	print_array_and_free(array, i);
}
int
main(const int argc, const char *const *const argv)
{
	assert(argc == 2);

	read_file(argv[1]);
}
