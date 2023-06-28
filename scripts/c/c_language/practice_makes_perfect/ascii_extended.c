//bin/true; exec ccompile "$0" -- "$@"

/* Print The extended ASCII characters. */

#include <locale.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
	for (int i = 32; i < 256; ++i) {
		printf("%d: %lc\n",i, i);
	}
}
