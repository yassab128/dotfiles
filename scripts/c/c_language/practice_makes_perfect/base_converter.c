//bin/true; exec ccompile "$0" -- "$@"

/*
 * Print argv[1] in other famous numeral systems.
 * Binary, octal, decimal and hexadecimal are supported.
*/

#include <stdio.h>
#include <stdlib.h>

#define BINARY_NUMBER_DIGITS_LIMIT 1024

int binary_to_decimal(char *);

void int_to_binary(int, char *);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Use the Source, Luke!\n");
		return 1;
	}
	char *string = argv[1];

	int number;
	if (string[0] == '0' && string[1] == 'x') {
		number = (int) strtol(string, 0, 16);
	} else if (string[0] == '0' && string[1] == 'b') {
		number = binary_to_decimal(string);
	} else if (string[0] == '0') {
		number = (int) strtol(string, 0, 8);
	} else {
		number = atoi(string);
	}

	printf("hex: 0x%x\n", number);
	printf("dec: %d\n", number);
	printf("oct: 0%o\n", number);

	char int_in_binary[BINARY_NUMBER_DIGITS_LIMIT] = { 0 };
	int_to_binary(number, int_in_binary);
	printf("bin: 0b%s\n", int_in_binary);
}

void int_to_binary(int number, char *binary_number)
{
	int index = 0;
	do {
		for (int i = index - 1; i >= 0; --i) {
			binary_number[i + 1] = binary_number[i];
		}
		++index;
		if (number % 2 == 0) {
			binary_number[0] = '0';
		} else {
			binary_number[0] = '1';
		}
	} while(number >>= 1);
}

int binary_to_decimal(char *str)
{
	int decimal = 0;

	int str_size;
	for (str_size = 0; str[str_size]; ++str_size) {
		if (!(str[str_size])) {
			break;
		}
	}

	for (int i = str_size - 1, two_power = 1, j;
			i > 1;
			--i, two_power <<= 1) {
		j = str[i] == '1' ? 1 : 0;
		decimal += two_power * j;
	}
	return decimal;
}
