//bin/true; exec ccompile "$0" -- "$@"

/* Check whether argv[1] is a palindrome. */

#include <stdio.h>
#include <string.h>

int is_palindrome(char *);
int is_palindrome(char *string)
{
	int string_length = (int) strlen(string);

	if (string_length % 2 == 0) {
		return(0);
	}

	int half = (int) string_length / 2;
	for (int i = 0; i <= half; ++i) {
		if (string[i] != string[string_length - i - 1]) {
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

	char *str = argv[1];
	if (is_palindrome(str)) {
		printf("True\n");
	}
}
