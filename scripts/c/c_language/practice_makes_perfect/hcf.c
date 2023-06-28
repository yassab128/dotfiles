//bin/true; exec ccompile "$0" -- "$@"

/* Print HCF of multiple number. */

#include <stdio.h>
#include <stdlib.h>

int hcf(int x, int y);
int hcf(int x, int y)
{

	int lowest_common_factor = 1;
	for (int i = 2; i <= x && i <= y; ++i) {
		if (x % i == 0 && y % i == 0) {
			lowest_common_factor = i;
		}
	}
	return(lowest_common_factor);
}


int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Use the Source, Luke!\n");
		return 1;
	}

	int previous = atoi(argv[1]);
	for (int i = 2; i < argc; ++i) {
		previous = hcf(previous, atoi(argv[i]));
	}
	printf("%d\n", previous);
	return 0;
}
