//bin/true; exec ccompile "$0" -- "$@"

/*
 * Remove all occurances of char argv[1] from string argv[2] and print it.
 * Adapted from https://stackoverflow.com/a/9895318/12493245
 */

#include <stdio.h>

void remove_all_chars(char*, char);
void remove_all_chars(char* str, char c) {
    char *pr = str;
	char *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

int main(int argc, char *argv[]) {
	if (argc < 3 || ! argv[1][0]) {
		printf("Use the Source, Luke!\n");
		return 1 ;
	}

    remove_all_chars(argv[2], argv[1][0]);
    printf("%s\n", argv[2]);
    return 0;
}
