//true; exec ccompile "$0" -lcurl -I/usr/include/gdk-pixbuf-2.0 -I\
//usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libpng16 -I\
//usr/include/libmount -D_GNU_SOURCE -I/usr/include/blkid\
//-pthread -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 "$@"

#include <curl/curl.h>
#include <libnotify/notify.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#define INOREADER_CLIENT_ID "999998025"
#define INOREADER_CLIENT_SECRET "YEAWn2F1y8bcw5T7UzrUsqBxhy2PArKO"
#define INOREADER_REFRESH_TOKEN "38be6dcbb5dfedbf8bc7cb41ca2a0ce6b50e1375"

#define GMAIL_CLIENT_ID "575205653021-ivlgjpvhdm0mp9id16fc3t4irhmfclfk."\
	"apps.googleusercontent.com"
#define GMAIL_CLIENT_SECRET "6OslLIoWUNYZ_u2qsydIvSA7"
#define GMAIL_REFRESH_TOKEN "1//0doIgp17m0RqECgYIARAAGA0SNwF-L9IryYKpd4OExs6"\
	"tUj9L9xPbxeDJLUKb9IESudJOHGDeZ6gT__uJEMjwOI_ziXLzSlZeFkw"
#define TEN_SECONDS 10000

static void extract_substring(char *pattern, char *string, char **substring) {
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, string, 2, matches, 0)) {
		quick_exit(1);
	}
	asprintf(substring, "%.*s", matches[1].rm_eo - matches[1].rm_so,
		string + matches[1].rm_so);
	regfree(&preg);
}

static void notify_send(char *message, unsigned short timeout)
{
	notify_init("m");
	NotifyNotification *notification = notify_notification_new(message, 0, 0);
	notify_notification_set_timeout(notification, timeout);
	notify_notification_show(notification, 0);
	g_object_unref(G_OBJECT(notification));
	notify_uninit();
}

static unsigned short gmail_parse(char *ptr, unsigned short size,
	unsigned short nmemb, char **unread_count)
{
	extract_substring("\"resultSizeEstimate\": ?([0-9]+)", ptr,
		unread_count);
	return size * nmemb;
}

static unsigned short inoreader_parse(char *ptr, unsigned short size,
	unsigned short nmemb, char **unread_count)
{
	extract_substring("\"count\": ?([0-9]+)", ptr,
		unread_count);
	return size * nmemb;
}

static unsigned char get_access_token(char *ptr, unsigned char size,
	unsigned char nmemb, char **userdata)
{
	extract_substring("\"access_token\": ?\"([^\"]+)", ptr,
		userdata);
	return size * nmemb;
}

static void inoreader()
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		return;
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
		return;
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			"https://www.inoreader.com/reader/api/0/unread-count");
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token_header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	char *unread_count = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &unread_count);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inoreader_parse);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(access_token_header);

	if (*unread_count == '0') {
		return;
	}
	char *notification_message = 0;
	asprintf(&notification_message, "Inoreader: %s", unread_count);
	free(unread_count);
	notify_send(notification_message, TEN_SECONDS);
	free(notification_message);
}

static void gmail()
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		return;
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
		return;
	}

	/* Do not replace `\"` with `'`! */
	curl_easy_setopt(curl, CURLOPT_URL,
		"https://gmail.googleapis.com/gmail/v1/users/me/messages?"
		"fields=resultSizeEstimate&q=\"is:unread\"");
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token_header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gmail_parse);
	char *unread_count = 0;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &unread_count);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(access_token_header);

	if (*unread_count == '0') {
		return;
	}
	char *notification_message = 0;
	asprintf(&notification_message, "Gmail: %s", unread_count);
	free(unread_count);
	notify_send(notification_message, TEN_SECONDS);
	free(notification_message);
}

/*
#include <time.h>
static int my_sleep(long seconds)
{
     struct timespec req;
     req.tv_sec = seconds;
     req.tv_nsec = 0;
     return nanosleep(&req , 0);
}
*/

int main()
{
	inoreader();
	gmail();
}
