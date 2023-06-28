//bin/true; exec ccompile "$0" -- "$@"

/* Print the contents of the C source file. */

#include <stdio.h>
#define MAX_LINE_LENGTH 80
#define SOURCE_FILE __FILE__

int main()
{
	char line[MAX_LINE_LENGTH];
	FILE *file = fopen(SOURCE_FILE, "r");
	while (fgets(line, MAX_LINE_LENGTH, file)) {
		printf("%s", line);
	}
	fclose(file);
}
