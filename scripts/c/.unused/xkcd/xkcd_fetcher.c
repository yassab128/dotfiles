#if 0
# -fsanitize=address,undefined,leak \
printf "\e[1;1H\e[2J"
f="${0%.c}"
[ -f "$f" ] && : > "$f"
clang -Weverything -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
	-lcurl \
	-g -o "$f" "$0"
exec "$f" /sdcard/xkcd
#endif

#define RECOMMENDED_ALIGNMENT 16
struct request {
	char *body;
	unsigned long length;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

#include <curl/curl.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int get_response_body(char *ptr, unsigned int size,
	unsigned int nmemb, struct request *userdata)
{
	userdata->body = realloc(userdata->body, nmemb + userdata->length + 1);
	memcpy(userdata->body + userdata->length, ptr, nmemb + 1);
	userdata->length += nmemb;
	return size * nmemb;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		return 1;
	}
	CURL *curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL, "https://xkcd.com/info.0.json");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_body);
	struct request response;
	response.length = 0;
	response.body = calloc(1, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	regex_t preg;
	regcomp(&preg, "\"num\": ?([0-9]+)", REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, response.body, 2, matches, 0)) {
		quick_exit(1);
	}
	unsigned short comics_number = 0;
	sscanf(response.body + matches[1].rm_so, "%hu", &comics_number);
	regfree(&preg);
	free(response.body);

	curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_body);
	response.length = 0;
	response.body = calloc(1, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	char *url = 0;
	putchar('\n');
	for (unsigned short i = comics_number; i; --i) {
		asprintf(&url, "https://xkcd.com/%hu/info.0.json", i);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_perform(curl);
		printf("\r%hu", i);
		fflush(stdout);
		free(url);
	}
	curl_easy_cleanup(curl);

	regcomp(&preg, "\"img\": ?\"([^\"]+)", REG_EXTENDED);
	char **urls = malloc((comics_number + 1) * sizeof(char *));
	urls[comics_number] = 0;

	unsigned short j = 0;
	for (char *cursor = response.body;
			!regexec(&preg, cursor, 2, matches, 0);
			cursor += matches[1].rm_eo) {
		asprintf(urls + j, "%.*s",
			(int)(matches[1].rm_eo - matches[1].rm_so),
			cursor + matches[1].rm_so);
		++j;
	}
	regfree(&preg);
	free(response.body);

	curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 0);

	FILE *fp = 0;
	char *file_path = 0;
	for (char **ptr = urls; *ptr; ++ptr) {
		asprintf(&file_path, "%s/%s", argv[1],
			strrchr(*ptr, '/') + 1);
		fp = fopen(file_path, "wbe");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_URL, *ptr);
		curl_easy_perform(curl);
		fclose(fp);
		free(*ptr);

		printf("%s\n", file_path);
		free(file_path);
	}
	curl_easy_cleanup(curl);

	free(urls);
	printf("END!\n");
}
