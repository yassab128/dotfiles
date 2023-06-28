#include <curl/curl.h>
#include <stdio.h>

int main()
{
	size_t b = 5;
	printf("%hhd\n", b);
	printf("%hhu\n", CURL_WRITEFUNC_PAUSE);
}
