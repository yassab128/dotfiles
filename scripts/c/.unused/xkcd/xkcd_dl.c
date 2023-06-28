#if 0
#- fsanitize = address, undefined, leak printf "\e[1;1H\e[2J"
tmux clear-history
f="${0%.c}"
[ -f "$f" ] && : > "$f"
clang -Weverything -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
	-lcurl \
	-g -o "$f" "$0"
clang-tidy --checks='*' "$0"
exec "$f" /sdcard/xkcd
#endif

#include <curl/curl.h>
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THREADS_MAX 256
#define DECIMAL_BASE 10

#define LATEST_XKCD "https://xkcd.com/info.0.json"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define DIE(x)                                                                 \
	fputs("Died at " __FILE__ " line " TOSTRING(__LINE__) ".\n", stderr);  \
	quick_exit(x)

#define RECOMMENDED_ALIGNMENT 16
struct request {
	char *body;
	unsigned long length;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static const char *output_dir;

static unsigned int get_response_body(const char *ptr, const unsigned int size,
				      const unsigned int nmemb,
				      struct request *userdata)
{
	userdata->body = realloc(userdata->body, nmemb + userdata->length);
	memcpy(userdata->body + userdata->length, ptr, nmemb);
	userdata->length += nmemb;
	return size * nmemb;
}

static void *download_file(void *arg)
{
	const unsigned long comic_id = (unsigned long)arg;
	CURL *curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}
	char *url = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_body);
	struct request response = {.body = malloc(1), .length = 0};
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	asprintf(&url, "https://xkcd.com/%lu/info.0.json", comic_id);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_perform(curl);
	free(url);
	curl_easy_cleanup(curl);

	regex_t preg;
	regcomp(&preg, "\"img\": ?\"([^\"]+)", REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, response.body, 2, matches, 0)) {
		regfree(&preg);
		free(response.body);
		return 0;
	}
	asprintf(&url, "%.*s", (int)(matches[1].rm_eo - matches[1].rm_so),
		 response.body + matches[1].rm_so);
	regfree(&preg);
	free(response.body);

	curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}

	char *file_path = 0;
	asprintf(&file_path, "%s/%lu_%s", output_dir, comic_id,
		 strrchr(url, '/') + 1);
	FILE *fp = fopen(file_path, "wbe");
	free(file_path);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_perform(curl);

	fclose(fp);
	free(url);

	curl_easy_cleanup(curl);

	return 0;
}

static unsigned long get_comics_number()
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL, LATEST_XKCD);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_body);
	struct request response = {.body = malloc(1), .length = 0};
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	regex_t preg;
	regcomp(&preg, "\"num\": ?\"?([0-9]+)", REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, response.body, 2, matches, 0)) {
		DIE(1);
	}
	const unsigned long total =
	    strtoul(response.body + matches[1].rm_so, 0, DECIMAL_BASE);
	regfree(&preg);
	free(response.body);

	return total;
}

int main(const int argc, const char **argv)
{
	if (argc != 2) {
		return 1;
	}
	output_dir = argv[1];

	const unsigned long comics_number = get_comics_number();
	pthread_t thread_id[THREADS_MAX];

	for (unsigned long j = 0, k = comics_number, l = 0; k;) {
		j = k > THREADS_MAX ? THREADS_MAX : k;
		for (unsigned long i = 0; i < j; ++i) {
			pthread_create(thread_id + i, 0, download_file,
				       (void *)k);
			--k;
		}

		for (unsigned long i = 0; i < j; ++i) {
			pthread_join(thread_id[i], 0);
			fprintf(stderr, "\r%lu/%lu", ++l, comics_number);
		}
	}
	putc('\n', stderr);
}
