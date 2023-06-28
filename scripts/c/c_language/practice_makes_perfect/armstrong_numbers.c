//bin/true; exec ccompile "$0" -- "$@"

/* Print Narcissistic numbers from 0 to argv[1]. */

#include <stdio.h>
#include <stdlib.h>

#define MAX_DIGITS 1024

void decompose_number(int, int*);
void decompose_number(int num, int* arr)
{
	int dig = 0;

	int i = 1;
	while(num > 0)
	{
		dig = num % 10;
		arr[i++] = dig;
		num = num / 10;
	}

	/* index 0 corresponds to the array size. */
	arr[0] = i;
}

int power(int, int);
int power(int x, int y)
{
	int result = 1;
	for (int i = 0; i < y; ++i) {
		result *= x;
	}
	return(result);
}

int is_narcissistic_number(int);
int is_narcissistic_number(int number)
{
	int digits[MAX_DIGITS];
	decompose_number(number, digits);

	int digits_number = digits[0] - 1;
	for (int i = 1; i <= digits_number; ++i) {
		number -= power(digits[i], digits_number);
	}
	return number == 0 ? 1 : 0;
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Use the Source, Luke!\n");
		return(0);
	}

	int upper_bound = atoi(argv[1]);
	for (int i = 0; i <= upper_bound; ++i) {
		if(is_narcissistic_number(i)) {
			printf("%d\n", i);
		}
	}
}
