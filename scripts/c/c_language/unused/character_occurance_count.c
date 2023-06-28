//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>

#define N 16384
#define M 64

/*
 * Print the number of occurences of each UTF-8 character.
 * CJK languages are not supported.
 * Tested with ASCII, ASCII extended and some non-CJK languages.
*/

int (*(process_string)(char *))[N];

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Use the Source, Luke!\n");
		return 1;
	}

	int (*occurances)[N];
	occurances = process_string(argv[1]);

	char ascii[2];
	char not_ascii[3];
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (occurances[i][j] != 0) {
				if (i == 0) {
					ascii[0] = (char) j;
					ascii[1] = '\0';
					printf("%s: %d\n",
							ascii, occurances[i][j]);
				} else {
					not_ascii[0] = (char) -i;
					not_ascii[1] = (char) -j;
					not_ascii[2] = 0;
					printf("%s: %d\n",
							not_ascii, occurances[i][j]);
				}
			}
		}
	}
}

int (*(process_string)(char *string))[N]
{
	printf("%s\n", string);
	int is_ascii = 1;
	static int array[M][N] = { {0,0} };
	int byte = 0;
	for (int i = 0; string[i]; ++i) {
		if (is_ascii && string[i] < 0) {
			byte = string[i];
			is_ascii = 0;
			continue;
		}

		if (is_ascii) {
			++array[0][+string[i]];
		} else {
			++array[-byte][-string[i]];

			is_ascii = 1;
		}
	}
	return array;
}
