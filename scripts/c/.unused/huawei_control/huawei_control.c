#include "../get_router_ip/get_router_gateway.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../my_macros.h"

struct string {
	char *data;
	unsigned long length;
};

static struct string base_url;

static struct string
get_base_url()
{
	const int gateway = get_router_ipv4_gateway();
	CHECK(gateway == -1);
	char *str;
	const int asprintf_status = asprintf(
	 &str, "http://%d.%d.%d.%d/api/", INT_TO_IPV4_CONVERTER(gateway));
	CHECK(asprintf_status == -1);
	return (struct string){str, (unsigned long)asprintf_status};
}

static char *
build_url(const char *path, const unsigned long path_size)
{
	const unsigned long url_size = base_url.length + path_size;
	char *url = malloc(url_size);
	CHECK(!url);
	memcpy(url, base_url.data, base_url.length);
	memcpy(url + base_url.length, path, path_size);
	return url;
}

static unsigned int
get_response_data(const char *ptr, const unsigned int size,
 const unsigned int nmemb, struct string *userdata)
{
	userdata->data = realloc(userdata->data, nmemb + userdata->length + 1);
	CHECK(!userdata->data);
	memcpy(userdata->data + userdata->length, ptr, nmemb + 1);
	userdata->length += nmemb;
	return size * nmemb;
}

static unsigned char
extract_xml(const struct string haystack, const char *needle,
 const unsigned long needle_length, struct string *substring)
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

static struct string
get_initial_credentials()
{
	CURL *curl = curl_easy_init();
	CHECK(!curl);

	char *url = build_url(STR_WITH_SIZE("webserver/SesTokInfo"));
	CHECK(curl_easy_setopt(curl, CURLOPT_URL, url));
	free(url);

	struct string response = {.data = malloc(1), .length = 0};
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response));
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data));

	CHECK(curl_easy_perform(curl));
	curl_easy_cleanup(curl);

	struct string ses_info;
	extract_xml(response, STR_WITH_STRLEN("<SesInfo>"), &ses_info);
	free(response.data);

	return ses_info;
}

static struct string
send_request(const char *subdirectory, const unsigned long subdirectory_size,
 const char *ses_info)
{
	CURL *curl = curl_easy_init();
	CHECK(!curl);
	curl_easy_setopt(curl, CURLOPT_COOKIE, ses_info);

	char *url = build_url(subdirectory, subdirectory_size);
	CHECK(curl_easy_setopt(curl, CURLOPT_URL, url));
	free(url);

	struct string response = {.data = malloc(1), .length = 0};
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response));
	CHECK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data));

	CHECK(curl_easy_perform(curl));
	curl_easy_cleanup(curl);

	return response;
}

static struct string
grab_response(const char *subdirectory, const unsigned long subdirectory_size,
 struct string *substring, const char *needle,
 const unsigned long needle_length, struct string *ses_info)
{
	struct string response =
	 send_request(subdirectory, subdirectory_size, ses_info->data);
	if (extract_xml(response, needle, needle_length, substring) ==
	 EXIT_FAILURE) {
		free(ses_info->data);
		*ses_info = get_initial_credentials();
		free(response.data);
		response =
		 send_request(subdirectory, subdirectory_size, ses_info->data);
		extract_xml(response, needle, needle_length, substring);
	}
	return response;
}

static void
get_netspeed(struct string *ses_info, int *netload)
{
	struct string download_rate;
	const struct string response =
	 grab_response(STR_WITH_SIZE("monitoring/traffic-statistics"),
	  &download_rate, STR_WITH_STRLEN("<CurrentDownloadRate>"), ses_info);

	struct string upload_rate;
	extract_xml(
	 response, STR_WITH_STRLEN("<CurrentUploadRate>"), &upload_rate);

	free(response.data);
	netload[0] = atoi(download_rate.data);
	free(download_rate.data);
	netload[1] = atoi(upload_rate.data);
	free(upload_rate.data);
}

static int
get_network_users(struct string *ses_info)
{
	struct string users_num_str;
	const struct string response =
	 grab_response(STR_WITH_SIZE("monitoring/status"), &users_num_str,
	  STR_WITH_STRLEN("<CurrentWifiUser>"), ses_info);
	free(response.data);
	int users_num = atoi(users_num_str.data);
	free(users_num_str.data);
	return users_num;
}

#if 0
static void
print_memory_stuff()
{
	FILE *fp = fopen("/proc/self/smaps_rollup", "r");
	CHECK(!fp);
	char rss[16];
	fscanf(fp, "%*[^\n]\n%*[^0-9]%s", rss);
	fclose(fp);
	fputs(rss, stderr);
	putc('\n', stderr);
}
#endif

int
main()
{
	base_url = get_base_url();
	struct string ses_info = get_initial_credentials();

	// repeat_start
	ses_info.data[20] = 'g';
	int netload[2];
	get_netspeed(&ses_info, netload);
	int network_users = get_network_users(&ses_info);
	char temp;
	printf("%.1f%c|%d|%.1f%c\n", HUMAN_SIZE(netload[0], temp),
	 network_users, HUMAN_SIZE(netload[1], temp));
	// repeat_end

	free(ses_info.data);
	free(base_url.data);
}
