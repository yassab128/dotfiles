#include "../get_router_ip/get_router_gateway.h"
#include <curl/curl.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../my_macros.h"

struct string {
	char *data;
	unsigned long length;
};

static struct string g_base_url;
static struct string g_ses_info;

static unsigned char g_users_num;
static int g_netspeed[2];

static void
get_base_url()
{
	const int gateway = get_router_ipv4_gateway();
	int asprintf_status;
	CHECK(gateway == -1);
	asprintf_status = asprintf(&g_base_url.data, "http://%d.%d.%d.%d/api/",
	 INT_TO_IPV4_CONVERTER(gateway));
	CHECK(asprintf_status == -1);
	g_base_url.length = (unsigned long)asprintf_status;
}

static char *
build_url(const char *const path, const unsigned long path_size)
{
	const unsigned long url_size = g_base_url.length + path_size;
	char *url = malloc(url_size);
	CHECK(!url);
	memcpy(url, g_base_url.data, g_base_url.length);
	memcpy(url + g_base_url.length, path, path_size);
	return url;
}

static unsigned int
get_response_data(const char *const ptr, const unsigned int size,
 const unsigned int nmemb, struct string *const userdata)
{
	userdata->data = realloc(userdata->data, nmemb + userdata->length + 1);
	CHECK(!userdata->data);
	memcpy(userdata->data + userdata->length, ptr, nmemb + 1);
	userdata->length += nmemb;
	return size * nmemb;
}

static unsigned char
extract_xml(const struct string haystack, const char *const needle,
 const unsigned long needle_length, struct string *const substring)
{
	char *str =
	 memmem(haystack.data, haystack.length, needle, needle_length);

	if (!str) {
		return EXIT_FAILURE;
	}

	str += needle_length;

	substring->length =
	 (unsigned long)((char *)memchr(str, '<',
			  haystack.length -
			   (unsigned long)(str - haystack.data)) -
	  str);

	str[substring->length] = 0;

	substring->data = malloc(substring->length + 1);
	CHECK(!substring->data);
	memcpy(substring->data, str, substring->length + 1);
	return EXIT_SUCCESS;
}

static void
get_initial_credentials()
{
	struct string response;
	char *const url = build_url(STR_WITH_SIZE("webserver/SesTokInfo"));
	CURL *const curl = curl_easy_init();
	CHECK(!curl);

	CHECK(curl_easy_setopt(curl, CURLOPT_URL, url));
	free(url);

	response.data = malloc(1);
	CHECK(!response.data);
	response.length = 0;

	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response));
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data));

	CHECK(curl_easy_perform(curl));
	curl_easy_cleanup(curl);

	extract_xml(response, STR_WITH_STRLEN("<SesInfo>"), &g_ses_info);
	free(response.data);
}

static struct string
send_request(const char *subdirectory, const unsigned long subdirectory_size)
{
	struct string response;
	char *const url = build_url(subdirectory, subdirectory_size);
	CURL *const curl = curl_easy_init();
	CHECK(!curl);
	curl_easy_setopt(curl, CURLOPT_COOKIE, g_ses_info.data);

	CHECK(curl_easy_setopt(curl, CURLOPT_URL, url));
	free(url);

	response.data = malloc(1);
	CHECK(!response.data);
	response.length = 0;

	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response));
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data));

	CHECK(curl_easy_perform(curl));
	curl_easy_cleanup(curl);

	return response;
}

static void *
get_network_users(void *arg)
{
	struct string users_num_str;
	const struct string response =
	 send_request(STR_WITH_SIZE("monitoring/status"));
	(void)arg;

	if (extract_xml(
	     response, STR_WITH_STRLEN("<CurrentWifiUser>"), &users_num_str)) {
		free(response.data);
		errno = EPERM;
		return 0;
	}
	free(response.data);
	g_users_num = (unsigned char)atoi(users_num_str.data);
	free(users_num_str.data);
	if (!g_users_num) {
		errno = EPERM;
	}
	return 0;
}

static void *
get_netspeed(void *arg)
{
	struct string net_rate;
	const struct string response =
	 send_request(STR_WITH_SIZE("monitoring/traffic-statistics"));
	(void)arg;

	if (extract_xml(
	     response, STR_WITH_STRLEN("<CurrentDownloadRate>"), &net_rate)) {
		free(response.data);
		errno = EPERM;
		return 0;
	}
	g_netspeed[0] = atoi(net_rate.data);
	free(net_rate.data);
	if (extract_xml(
	     response, STR_WITH_STRLEN("<CurrentUploadRate>"), &net_rate)) {
		free(response.data);
		errno = EPERM;
		return 0;
	}
	free(response.data);
	g_netspeed[1] = atoi(net_rate.data);
	free(net_rate.data);
	return 0;
}

struct human_size {
	double value;
	char *unit;
};

static struct human_size
convert_size(int size_in_bytes)
{
	struct human_size size;
	if (size_in_bytes > 1048576) {
		size.value = (double)size_in_bytes / 1048576;
		size.unit = "MB/s";
	} else if (size_in_bytes > 1024) {
		size.value = (double)size_in_bytes / 1024;
		size.unit = "KB/s";
	} else {
		size.value = (double)size_in_bytes;
		size.unit = "B/s";
	}
	return size;
}

#define ENABLE_LOOPING 1

int
main()
{
	struct human_size download_rate_readable;
	struct human_size upload_rate_readable;
	pthread_t thread_id[2];

#if ENABLE_LOOPING
	const char *printf_format = isatty(0)
	 ? "\r\033[0K%.1f %s | %hhu | %.1f %s"
	 : "%.1f %s | %hhu | %.1f %s\n";
#else
	const char *printf_format = "%.1f %s | %hhu | %.1f %s\n";
#endif

	get_base_url();
	get_initial_credentials();
#if ENABLE_LOOPING
	for (;;) {
#endif
		CHECK(
		 pthread_create(thread_id, 0, get_network_users, (void *)0));
		CHECK(
		 pthread_create(thread_id + 1, 0, get_netspeed, (void *)0));
		CHECK(pthread_join(thread_id[0], 0));
		CHECK(pthread_join(thread_id[1], 0));
#if ENABLE_LOOPING
		if (errno == EPERM) {
			errno = 0;
			free(g_ses_info.data);
			get_initial_credentials();
			continue;
		}
#endif
		download_rate_readable = convert_size(g_netspeed[0]);
		upload_rate_readable = convert_size(g_netspeed[1]);
		printf(printf_format, download_rate_readable.value,
		 download_rate_readable.unit, g_users_num,
		 upload_rate_readable.value, upload_rate_readable.unit);
		fflush(stdout);
#if ENABLE_LOOPING
		sleep(2);
	}
#endif

#if !ENABLE_LOOPING
	free(g_ses_info.data);
	free(g_base_url.data);
#endif
	return EXIT_SUCCESS;
}
