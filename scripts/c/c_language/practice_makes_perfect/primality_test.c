//bin/true; exec ccompile "$0" -- "$@"

/* Print prime numbers between 0 and argv[1]. */

#include <stdio.h>
#include <stdlib.h>

int is_prime(int);
int is_prime(int number)
{
	if (number == 0 || number == 1) {
		return(0);
	}

	for (int i = 2; i * i <= number; ++i) {
		if (number % i == 0) {
			return(0);
		}
	}
	return(1);
}


int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Use the Source, Luke!\n");
		return(0);
	}

	int high = atoi(argv[1]);

	for (int i = 0; i < high; ++i) {
		if (is_prime(i)) {
			printf("%d\n", i);
		}
	}
}
