#if 0
exec ccompile "$0" -D_GNU_SOURCE -I/usr/include/dbus-1.0/ \
	-I/usr/lib/dbus-1.0/include/ -lcurl -ldbus-1 -- "$@"
#endif

#include <curl/curl.h>
#include <notify.h>
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INOREADER_CLIENT_ID "999998025"
#define INOREADER_CLIENT_SECRET "YEAWn2F1y8bcw5T7UzrUsqBxhy2PArKO"
#define INOREADER_REFRESH_TOKEN "b26bdec943091e739e6930f766ee44c5849479ea"

#define GMAIL_CLIENT_ID "575205653021-ivlgjpvhdm0mp9id16fc3t4irhmfclfk."\
	"apps.googleusercontent.com"
#define GMAIL_CLIENT_SECRET "6OslLIoWUNYZ_u2qsydIvSA7"
#define GMAIL_REFRESH_TOKEN "1//0doIgp17m0RqECgYIARAAGA0SNwF-L9IryYKpd4OExs6"\
	"tUj9L9xPbxeDJLUKb9IESudJOHGDeZ6gT__uJEMjwOI_ziXLzSlZeFkw"

#define DEBUG 0
#define TRACE(...) if (DEBUG) {\
	fprintf(stderr, "\x1B[31m%s:%d:\x1B[0m ", __func__, __LINE__);\
	fprintf(stderr, __VA_ARGS__);\
}

#define DIE fprintf(stderr, "Died at %s() line %u\n", __func__, __LINE__);\
	quick_exit

#define RECOMMENDED_ALIGNMENT 16
struct unread {
	unsigned short gap;
	unsigned short value;
	char *pattern;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static void extract_substring(char *pattern, char *string, char **substring) {
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, string, 2, matches, 0)) {
		DIE(1);
	}
	asprintf(substring, "%.*s", matches[1].rm_eo - matches[1].rm_so,
		string + matches[1].rm_so);
	regfree(&preg);
}
static unsigned short gmail_parse(char *ptr, unsigned short size,
	unsigned short nmemb, unsigned short *unread_count)
{
	TRACE("%s\n", ptr)
	char *unread_count_str = 0;
	extract_substring("\"resultSizeEstimate\": ?([0-9]+)", ptr,
		&unread_count_str);
	*unread_count = (unsigned short)atoi(unread_count_str);
	free(unread_count_str);
	return size * nmemb;
}

static unsigned short inoreader_parse(char *ptr, unsigned short size,
	unsigned short nmemb, unsigned short *unread_count)
{
	TRACE("%s\n", ptr)
	char *unread_count_str = 0;
	extract_substring("\"count\": ?([0-9]+)", ptr,
		&unread_count_str);

	*unread_count = (unsigned short)atoi(unread_count_str);
	free(unread_count_str);
	return size * nmemb;
}

static unsigned char get_access_token(char *ptr, unsigned char size,
	unsigned char nmemb, char **userdata)
{
	TRACE("token: %s\n", ptr)
	extract_substring("\"access_token\": ?\"([^\"]+)", ptr,
		userdata);
	return size * nmemb;
}

static unsigned short inoreader()
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			"https://www.inoreader.com/oauth2/token");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
		"client_id=" INOREADER_CLIENT_ID "&client_secret="
		INOREADER_CLIENT_SECRET "&grant_type=refresh_token&"
		"refresh_token=" INOREADER_REFRESH_TOKEN);
	char *access_token = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_access_token);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	char *access_token_header = 0;
	asprintf(&access_token_header, "Authorization:Bearer %s", access_token);
	free(access_token);

	curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			"https://www.inoreader.com/reader/api/0/unread-count");
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token_header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	unsigned short unread_count = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &unread_count);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inoreader_parse);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(access_token_header);
	return unread_count;
}

static void *gmail(void *unread_count)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			"https://oauth2.googleapis.com/token");

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
		"client_id=" GMAIL_CLIENT_ID "&client_secret="
		GMAIL_CLIENT_SECRET "&grant_type=refresh_token&"
		"refresh_token=" GMAIL_REFRESH_TOKEN);
	char *access_token = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_access_token);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	char *access_token_header = 0;
	asprintf(&access_token_header, "Authorization:Bearer %s", access_token);
	free(access_token);

	curl = curl_easy_init();
	if (!curl) {
		DIE(1);
	}

	/* Do not replace `\"` with `'`! */
	curl_easy_setopt(curl, CURLOPT_URL,
		"https://gmail.googleapis.com/gmail/v1/users/me/messages?"
		"fields=resultSizeEstimate&q=\"is:unread\"");
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token_header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gmail_parse);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, unread_count);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	free(access_token_header);
	/* return unread_count; */
	return 0;
}

#define NOTIFICATION_TIMEOUT 10000
int main()
{
	pthread_t thread_id = 0;
	unsigned short gmail_unread_count = 0;
	pthread_create(&thread_id, 0, gmail, &gmail_unread_count);
	unsigned short inoreader_unread_count = inoreader();
	pthread_join(thread_id, 0);
	/* printf("%d | %d\n", inoreader_unread_count, gmail_unread_count); */

	char *msg = 0;
	if (gmail_unread_count && !inoreader_unread_count) {
		asprintf(&msg, "Gmail: %d", gmail_unread_count);
	} else if (!gmail_unread_count && inoreader_unread_count) {
		asprintf(&msg, "Inoreader: %d", inoreader_unread_count);
	} else if (!gmail_unread_count && !inoreader_unread_count) {
		return 0;
	} else {
		asprintf(&msg, "Gmail: %d | Inoreader: %d", gmail_unread_count,
			inoreader_unread_count);
	}
	notify_send(msg, 0, NOTIFICATION_TIMEOUT);
	free(msg);
}
