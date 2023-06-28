//bin/true; exec ccompile "$0" -- "$@"

/* Swap argv[] elements */

#include <stdio.h>

void swap(char **str1, char **str2)
{
    char *temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Use the Source, Luke!\n");
		return 1;
	}
	for (int i = 1; i < argc - i; ++i) {
		swap(&argv[i], &argv[argc - i]);
	}
	for (int i = 1; i < argc; ++i) {
		printf("arr[%d] = %s\n", i, argv[i]);
	}
}
