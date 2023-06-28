#if 0
true; exec ccompile "$0"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int my_asprintf(char **ret, char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int count = vsnprintf(0, 0, format, ap);
	va_end(ap);
	*ret = malloc((unsigned)count + 1);
	va_start(ap, format);
	vsnprintf(*ret, (unsigned)count + 1, format, ap);
	va_end(ap);
	return count;
}

int main() {
	char *shit = 0;
	int a = 4;
	char *str = "dir";
	int code = my_asprintf(&shit, "heil %d %s", a, str);
	printf("%s | %d\n", shit, code);
	free(shit);
}
