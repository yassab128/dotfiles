//bin/true; exec ccompile "$0" -- "$@"

/* Print the greatest element of floats in argv array. */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Use the Source, Luke!\n");
		return 1;
	}

	double max = strtod(argv[1], 0);
	for (int i = 2; i < argc; ++i) {
		if (strtod(argv[i], 0) > max) {
			max = strtod(argv[i], 0);
		}
	}

	/* %g drops trailing zeros i.e. print 147.1 instead of 147.1000000 */
	printf("%g", max);
}
