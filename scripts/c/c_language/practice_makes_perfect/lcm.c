//bin/true; exec ccompile "$0" -- "$@"

/* Print LCM of multiple numbers i.e. argv[i] */

#include <stdio.h>
#include <stdlib.h>

int get_lcm(int, int);
int get_lcm(int x, int y)
{
	int max = x > y ? x : y;
	int m = x * y;
	int lcm = -1;
	for (int i = max; i <= m; ++i) {
		if (i % x == 0 && i % y == 0) {
			lcm = i;
			break;
		}
	}
	return(lcm);
}

int main(int argc, char *argv[])
{
	if (argc <= 2) {
		printf("Use the Source, Luke!\n");
		return(0);
	}

	int previous = atoi(argv[1]);
	for (int i = 2; i < argc; ++i) {
		previous = get_lcm(previous, atoi(argv[i]));
	}
	printf("%d\n", previous);
}
