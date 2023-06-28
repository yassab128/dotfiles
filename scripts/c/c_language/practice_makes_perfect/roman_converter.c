//bin/true; exec ccompile "$0" -- "$@"

/*
* Convert argv[1] to Roman numerals if it is in decimal digits and vice versa.
* argv[1] must not excede 65535 or
* MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMDXXXV
*/

#include <stdio.h>
#include <stdlib.h>

/* Max characters in a Roman number string that is less or equal to 65535. */
#define MAX_UNSIGNED_SHORT 71

void intToRoman(unsigned short, char *);
unsigned short romanToInt(char *);

int main(int argc, char *argv[]) {
	if (argc == 1 || ! argv[1][0]) {
		printf("Use the Source, Luke!\n");
		return 1 ;
	}

	unsigned short integer_number;
	char roman_number[MAX_UNSIGNED_SHORT];

	char is_roman = 1;
	char is_integer = 1;

	for (unsigned short i = 0; argv[1][i]; ++i) {
		if (argv[1][i] < 48 || argv[1][i] > 57) {
			is_integer = 0;
		}

		if (argv[1][i] != 'I' || argv[1][i] != 'V' || argv[1][i] != 'X' ||
			argv[1][i] != 'L' || argv[1][i] != 'C' || argv[1][i] != 'D' ||
			argv[1][i] != 'M' ) {
			is_roman = 0;
		}
	}

	if (is_integer) {
		integer_number = (unsigned short) atoi(argv[1]);
		intToRoman(integer_number, roman_number);
		printf("%s\n", roman_number);

		integer_number = romanToInt(roman_number);
		printf("%d\n", integer_number);
	} else if (is_roman) {
		integer_number = romanToInt(argv[1]);
		printf("%d\n", integer_number);

		intToRoman(integer_number, roman_number);
		printf("%s\n", roman_number);
	} else {
		printf("Use the Source, Luke!\n");
		return 1 ;
	}
}

void intToRoman(unsigned short num, char *roman){
	for (unsigned short i = 0; num; ++i) {
		if (num >= 1000) {
		   roman[i] = 'M';
		   num -= 1000;
		} else if (num >= 900) {
		   roman[i] = 'C';
		   roman[++i] = 'M';
		   num -= 900;
		} else if (num >= 500) {
		   roman[i] = 'D';
		   num -= 500;
		} else if (num >= 400) {
		   roman[i] = 'C';
		   roman[++i] = 'D';
		   num -= 400;
		} else if (num >= 100) {
		   roman[i] = 'C';
		   num -= 100;
		} else if (num >= 90) {
		   roman[i] = 'X';
		   roman[++i] = 'C';
		   num -= 90;
		} else if (num >= 50) {
		   roman[i] = 'L';
		   num -= 50;
		} else if (num >= 40) {
		   roman[i] = 'X';
		   roman[++i] = 'L';
		   num -= 40;
		} else if (num >= 10) {
		   roman[i] = 'X';
		   num -= 10;
		} else if (num >= 9) {
		   roman[i] = 'I';
		   roman[++i] = 'X';
		   num -= 9;
		} else if (num >= 5) {
		   roman[i] = 'V';
		   num -= 5;
		} else if (num >= 4) {
		   roman[i] = 'I';
		   roman[++i] = 'V';
		   num -= 4;
		} else {
		   roman[i] = 'I';
		   num -= 1;
		}
	}
}

unsigned short romanToInt(char *roman) {
	unsigned short number = 0;
	char I_hold = 0;
	char X_hold = 0;
	char C_hold = 0;
	for (unsigned short i = 0; roman[i]; ++i) {
		if (I_hold == 0 && roman[i] == 'I') {
			I_hold = 1;
		} else if (I_hold) {
			if (roman[i] == 'V') {
				number += 4;
			} else if (roman[i] == 'X') {
				number += 9;
			} else {
				number += 2;
			}
			I_hold = 0;
		} else if (roman[i] == 'I') {
			number += 1;
		} else if (roman[i] == 'V') {
			number += 5;
		} else if (X_hold == 0 && roman[i] == 'X') {
			X_hold = 1;
		} else if (X_hold) {
			if (roman[i] == 'L') {
				number += 40;
			} else if (roman[i] == 'C') {
				number += 90;
			} else {
				number += 20;
			}
			X_hold = 0;
		} else if (roman[i] == 'X') {
			number += 10;
		} else if (roman[i] == 'L') {
			number += 50;
		} else if (C_hold == 0 && roman[i] == 'C') {
			C_hold = 1;
		} else if (C_hold) {
			if (roman[i] == 'D') {
				number += 400;
			} else if (roman[i] == 'M') {
				number += 900;
			} else {
				number += 200;
			}
			C_hold = 0;
		} else if (roman[i] == 'C') {
			number += 100;
		} else if (roman[i] == 'D') {
			number += 500;
		} else if (roman[i] == 'M') {
			number += 1000;
		}
	}
	if (I_hold) {
		number += 1;
	}
	if (X_hold) {
		number += 10;
	}
	if (C_hold) {
		number += 100;
	}

	return number;
}
