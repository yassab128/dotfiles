#include "get_gateway_address.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROUTER_URL "http://192.168.8.1/api/"
#define USERNAME "admin"
#define PASSWORD "SS33550336"

#define STRLEN(x) (sizeof(x) - 1)

#define SES_INFO_0_FILE P_tmpdir "/ses_info0"
#define TOK_INFO_0_FILE P_tmpdir "/tok_info0"

#define STR_WITH_STRLEN(x) x, STRLEN(x)

#define PRINT(x)\
	fprintf(stderr, "%s %lu %lu\n", (x).data, (x).size,\
		strlen((x).data ? (x).data : ""))

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CURL_EASY_PERFORM(x)\
	{\
		if (curl_easy_perform(x)) {\
			fputs("Connection error at " __FILE__\
				" line " TOSTRING(__LINE__) ".\n",\
				stderr);\
			quick_exit(1);\
		}\
		fputs("Connection succeeded at " __FILE__\
			" line " TOSTRING(__LINE__) ".\n",\
			stderr);\
	}

#define RECOMMENDED_ALIGNMENT 16
struct string {
	char *data;
	unsigned long size;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

struct number {
	double value;
	const char *unit;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static void
save_to_file(const struct string text, const char *destination)
{
	FILE *const fp = fopen(destination, "w");
	fwrite(text.data, 1, text.size, fp);
	fclose(fp);
}

static unsigned char
retrieve_from_file(struct string *variable, const char *destination)
{
	FILE *const fp = fopen(destination, "r");
	if (!fp) {
		fprintf(stderr, "Cannot read %s\n", destination);
		return 1;
	}
	unsigned int line_length;
	if (fscanf(fp, "%m[^\n]%n", &variable->data, &line_length) != 1) {
		fprintf(stderr, "%s is empty\n", destination);
		return 1;
	}
	fclose(fp);

	variable->size = (unsigned long)line_length;
	return 0;
}

static unsigned int
get_response_data(const char *ptr, const unsigned int size,
	const unsigned int nmemb, struct string *userdata)
{
	userdata->data = realloc(userdata->data, nmemb + userdata->size + 1);
	memcpy(userdata->data + userdata->size, ptr, nmemb + 1);
	userdata->size += nmemb;
	return size * nmemb;
}

static struct string
extract_xml(const struct string reg_string, const char *pattern,
	const unsigned long pattern_length)
{
	struct string substring = { 0 };

	char *str = memmem(reg_string.data, reg_string.size, pattern,
		pattern_length);
	if (!str) {
		fprintf(stderr, "%s is not a substring:\n%s\n", pattern,
			reg_string.data);
		return substring;
	}
	str += pattern_length;

	substring.size =
		(char *)memchr(str, '<',
		reg_string.size - (str - reg_string.data)) - str;
	substring.data = malloc(substring.size + 1);
	substring.data[substring.size] = 0;
	memcpy(substring.data, str, substring.size);
	return substring;
}

static void
str_prepend(const char *first_data, const unsigned long first_size,
	struct string *second)
{
	second->data = realloc(second->data, second->size + first_size + 1);
	memmove(second->data + first_size, second->data, second->size + 1);
	second->size += first_size;
	memcpy(second->data, first_data, first_size);
}

static void
get_ses_and_tok_info(const struct string response, struct string *credentials)
{
	struct string substring =
		extract_xml(response, STR_WITH_STRLEN("<SesInfo>"));
	if (substring.size) {
		if (credentials[0].data) {
			free(credentials[0].data);
		}
		credentials[0] = substring;
		str_prepend(STR_WITH_STRLEN("Cookie:"), &credentials[0]);
		save_to_file(credentials[0], SES_INFO_0_FILE);
	}

	substring = extract_xml(response, STR_WITH_STRLEN("<TokInfo>"));

	if (substring.size) {
		if (credentials[1].data) {
			free(credentials[1].data);
		}
		credentials[1] = substring;
		str_prepend(STR_WITH_STRLEN("__RequestVerificationToken:"),
			&credentials[1]);
		save_to_file(credentials[1], TOK_INFO_0_FILE);
	}
}

static char *
build_url(const struct string interface_gateway,
	const char *url_part, const unsigned long url_part_length)
{
	const char str_1[] = "http://";
	const char str_2[] = "/api/";
	unsigned long url_length = STRLEN(str_1) + interface_gateway.size +
		STRLEN(str_2) + url_part_length + 1;
	char *url = malloc(url_length);
	char *temp = url;
	memcpy(temp, str_1, STRLEN(str_1));
	memcpy(temp, interface_gateway.data, interface_gateway.size);
	temp += interface_gateway.size;
	memcpy(temp, str_2, STRLEN(str_2));
	temp += STRLEN(str_2);
	memcpy(temp, url_part, url_part_length + 1);
	return url;
}

static void
get_initial_credentials(const struct string gateway, struct string *credentials)
{
	CURL *curl = curl_easy_init();

	char *const url =
		build_url(gateway, STR_WITH_STRLEN("webserver/SesTokInfo"));
	curl_easy_setopt(curl, CURLOPT_URL, url);

	struct string response = {.data = malloc(1),.size = 0 };
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	CURL_EASY_PERFORM(curl);
	curl_easy_cleanup(curl);

	free(url);

	get_ses_and_tok_info(response, credentials);

	free(response.data);
}

static struct string
unprivileged_request(struct string *credentials, const char *api_url,
	const unsigned long api_url_length, const struct string gateway)
{
	CURL *curl = curl_easy_init();

	char *const url = build_url(gateway, api_url, api_url_length);

	curl_easy_setopt(curl, CURLOPT_URL, url);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	struct string response = {.data = malloc(1),.size = 0 };
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, credentials[0].data);
	// headers = curl_slist_append(headers, credentials[1].data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	CURL_EASY_PERFORM(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(url);
	return response;
}

int
main()
{
#if 1
	get_it();
	return 0;
#endif
	const struct string gateway = {.data = "192.168.8.2",
		.size = strlen("192.168.8.2")
	};
	struct string credentials[2] = { 0 };
	if (retrieve_from_file(&credentials[0], SES_INFO_0_FILE)) {
		retrieve_from_file(&credentials[1], TOK_INFO_0_FILE);
	}
	// print_info(credentials);
	get_initial_credentials(gateway, credentials);

	PRINT(credentials[0]);
	PRINT(credentials[1]);

	if (credentials[0].data) {
		free(credentials[0].data);
	}

	if (credentials[1].data) {
		free(credentials[1].data);
	}
}
