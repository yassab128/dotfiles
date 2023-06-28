//bin/true; exec ccompile "$0" -- "$@"

/* Split a string argv[2] to an array based on a delimiter argv[1] */

#include<stdio.h>
#include <string.h>

#define ELEMENTS 4096

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Use the Source, Luke!\n");
		return 1;
	}
	char *token = strtok(argv[2], argv[1]);
	char *arr[ELEMENTS] = { 0 };
	for (int i = 0; token; ++i) {
	  arr[i] = token;
	  token = strtok(0, argv[1]);
	}

	for (int i = 0; arr[i]; ++i) {
		printf("%s\n", arr[i]);
	}
}
