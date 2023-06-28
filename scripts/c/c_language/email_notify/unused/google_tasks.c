//true; exec ccompile "$0" -lcurl -I/usr/include/gdk-pixbuf-2.0 -I\
//usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libpng16 -I\
//usr/include/libmount -I/usr/include/blkid\
//-pthread -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 "$@"

#include <curl/curl.h>
#include <libnotify/notify.h>
#include <stdio.h>

#define TASKS_HEADERS "client_id=575205653021-ivlgjpvhdm0mp9id16fc3t4irhmf\
clfk.apps.googleusercontent.com&client_secret=6OslLIoWUNYZ_u2qsydIvSA7&\
grant_type=refresh_token&refresh_token=1//03NnWcUtGO5t0CgYIARAAGAMSNwF-\
L9Ir8oXECmxy-W4qYlztCSz-4nvhw-_UPexCISxs0YwE_7OVRlry2buA0qeLxxlFWcyOuLw"

#define GMAIL_HEADERS "client_id=575205653021-ivlgjpvhdm0mp9id16fc3t4irhmf\
clfk.apps.googleusercontent.com&client_secret=6OslLIoWUNYZ_u2qsydIvSA7&\
grant_type=refresh_token&refresh_token=1//0doIgp17m0RqECgYIARAAGA0SNwF-L9I\
ryYKpd4OExs6tUj9L9xPbxeDJLUKb9IESudJOHGDeZ6gT__uJEMjwOI_ziXLzSlZeFkw"

static void get_tasks_count(char *ptr, unsigned char size,
	unsigned char nmemb, unsigned char *userdata)
{
	for (unsigned short i = 0; ptr[i]; ++i) {
		if (ptr[i] == 'n') {
			++*userdata;
		}
	}
}

#define MAX_URL 101
static void construct_url(char urls[][MAX_URL], char *ptr,
	unsigned short start, unsigned short end)
{
	unsigned char j = 0;
	char id[MAX_URL] = {0};
	for (char *temp_string = "https://tasks.googleapis.com/tasks/v1/lists/";
		temp_string[j]; ++j) {
		id[j] = temp_string[j];
	}
	for (unsigned short i = start; i <= end; ++i) {
		id[i - start + j] = ptr[i];
	}
	j += 1 + end - start;
	unsigned short i = 0;
	for (char *temp_string = "/tasks?fields=items.kind";
	     temp_string[i]; ++j, ++i) {
	     id[j] = temp_string[i];
	}

	i = 0;
	while (urls[i][0]) {
		++i;
	}
	for (j = 0; id[j]; ++j) {
	     urls[i][j] = id[j];
	}
}


#define MAX_TASKLISTS 32
static void tasks_request(char *ptr, unsigned char size,
	unsigned char nmemb, char *userdata)
{
	char urls[MAX_TASKLISTS][MAX_URL] = {0};
	for (unsigned short i = 32, j = 0; ptr[i + 6]; ++i) {
		if (ptr[i] == ' ' && ptr[i + 1] == '"') {
			j = i + 2;
		} else if (ptr[i + 1] == '"' && ptr[i + 2] == 10) {
			construct_url(urls, ptr, j, i);
		}
	}

	unsigned char undone = 0;
	CURL *curl = curl_easy_init();
	if (!curl) {
	  return;
	}
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, userdata);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_tasks_count);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &undone); 
	for (unsigned short i = 0; urls[i][0]; ++i) {
		curl_easy_setopt(curl, CURLOPT_URL, urls[i]);
		curl_easy_perform(curl);
	}
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (undone) {
		printf("%d\n", undone);
	}
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

static char is_a_digit(char character)
{
	return (character >= '0' && character <= '9') ? 1 : 0;
}


static void gmail_request(char *ptr)
{
	char unread_count[9] = {0};

	unsigned char j = 0;
	for (char *temp_string = "Gmail: ";
		temp_string[j]; ++j) {
		unread_count[j] = temp_string[j];
	}

	if (ptr[26] == '0') {
		return;
	}

	for (unsigned char i = 26; is_a_digit(ptr[i]); ++i) {
		unread_count[i + j - 26] = ptr[i];
	}

	notify_send(unread_count, 10000);
}

static void get_gmail_access_token(char *string)
{
	char access_token[188] = {0};
	unsigned char i = 0;
	for (char *temp_string = "Authorization: Bearer ";
		temp_string[i]; ++i) {
		access_token[i] = temp_string[i];
	}
	for (unsigned char j = 0, k = 0; j < 4; ++k) {
		if (string[k] == '"') {
			++j;
			continue;
		}
		if (j == 3) {
			access_token[i] = string[k];
			++i;
		}
	}
	CURL *curl = curl_easy_init();
	if (!curl) {
	  return;
	}

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token); 
	/* Do not replace `\"` with `'`! */
	curl_easy_setopt(curl, CURLOPT_URL,
		"https://gmail.googleapis.com/gmail/v1/users/me/messages?"
		"fields=resultSizeEstimate&q=\"is:unread\"");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gmail_request);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void get_tasks_access_token(char *string)
{
	char access_token[188] = {0};
	unsigned char i = 0;
	for (char *temp_string = "Authorization: Bearer ";
		temp_string[i]; ++i) {
		access_token[i] = temp_string[i];
	}
	for (unsigned char j = 0, k = 0; j < 4; ++k) {
		if (string[k] == '"') {
			++j;
			continue;
		}
		if (j == 3) {
			access_token[i] = string[k];
			++i;
		}
	}
	CURL *curl = curl_easy_init();
	if (!curl) {
	  return;
	}

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, access_token);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token); 
	curl_easy_setopt(curl, CURLOPT_URL,
		"https://tasks.googleapis.com/tasks/v1/users/@me/lists"
		"?fields=items.id");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tasks_request);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void google_services_request(void) {
	CURL *curl = curl_easy_init();
	if (!curl) {
	  return;
	}

	curl_easy_setopt(curl, CURLOPT_URL,
			"https://oauth2.googleapis.com/token");

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, TASKS_HEADERS);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_tasks_access_token);
	curl_easy_perform(curl);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, GMAIL_HEADERS);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_gmail_access_token);
	curl_easy_perform(curl);

	curl_easy_cleanup(curl);
}

int main()
{
	google_services_request();
}
