#include "regex.h"
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

#define ROUTER_URL "http://192.168.8.1/api/"
#define USERNAME "admin"
#define PASSWORD "SS33550336"

#define STRLEN(x) (sizeof(x) - 1)

#define SES_INFO_0_FILE P_tmpdir "/ses_info0"
#define TOK_INFO_0_FILE P_tmpdir "/tok_info0"

#define SES_INFO_1_FILE P_tmpdir "/ses_info1"
#define TOK_INFO_1_FILE P_tmpdir "/tok_info1"

#define FIRSTNONCE                                                             \
	"0123456789012345678901234567890123456789012345678901234567890123"

#define BIN_HEX_FORMAT                                                         \
	"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"   \
	"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"   \
	"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"

#define REPEAT_32_TIMES(x)                                                     \
	x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9], x[10],     \
	    x[11], x[12], x[13], x[14], x[15], x[16], x[17], x[18], x[19],     \
	    x[20], x[21], x[22], x[23], x[24], x[25], x[26], x[27], x[28],     \
	    x[29], x[30], x[31]

#define STR_WITH_STRLEN(x) x, STRLEN(x)

#define PRINT(x) fprintf(stderr, "%s %lu %lu\n", x.data, x.size, strlen(x.data))

#define RECOMMENDED_ALIGNMENT 16
struct string {
	char *data;
	unsigned long size;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

struct number {
	double value;
	const char *unit;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

// static void my_sleep(const long seconds)
// {
// 	struct timespec tm;
// 	tm.tv_sec = seconds;
// 	tm.tv_nsec = 0;
// 	nanosleep(&tm, 0);
// }

static void save_to_file(const struct string text, const char *destination)
{
	FILE *fp = fopen(destination, "w");
	fwrite(text.data, 1, text.size, fp);
	fclose(fp);
}

static unsigned char retrieve_from_file(struct string *variable,
					const char *destination)
{
	FILE *fp = fopen(destination, "r");
	if (!fp) {
		fprintf(stderr, "Cannot read %s\n", destination);
		variable->data = calloc(1, 1);
		return 1;
	}
	unsigned int line_length;
	if (fscanf(fp, "%m[^\n]%n", &variable->data, &line_length) != 1) {
		fprintf(stderr, "%s is empty\n", destination);
		variable->data = calloc(1, 1);
		return 1;
	}
	fclose(fp);

	variable->size = (unsigned long)line_length;
	return 0;
}

static unsigned int get_response_data(const char *ptr, const unsigned int size,
				      const unsigned int nmemb,
				      struct string *userdata)
{
	userdata->data = realloc(userdata->data, nmemb + userdata->size + 1);
	memcpy(userdata->data + userdata->size, ptr, nmemb + 1);
	userdata->size += nmemb;
	return size * nmemb;
}

// static unsigned char extract_string(const char *reg_string,
// 				       struct string *substring,
// 				       const char *pattern)
// {
// 	regex_t preg;
// 	regcomp(&preg, pattern, REG_EXTENDED);
// 	regmatch_t matches[2];
// 	if (regexec(&preg, reg_string, 2, matches, 0)) {
// 		regfree(&preg);
// 		puts("Refreshing");
// 		fprintf(stderr, "Regexp %s has no match in:\n%s\n", pattern,
// 			reg_string);
// 		return 1;
// 	}
// 	substring->size = (unsigned long)(matches[1].rm_eo - matches[1].rm_so);
//
// 	substring->data = malloc(substring->size + 1);
// 	memcpy(substring->data, reg_string + matches[1].rm_so, substring->size);
// 	substring->data[substring->size] = 0;
// 	regfree(&preg);
// 	return 0;
// }

static unsigned char extract_string(const struct string reg_string,
				    const unsigned char include_needle,
				    struct string *substring,
				    const char *pattern,
				    const unsigned long pattern_length,
				    const char *sscanf_format)
{
	char *str =
	    memmem(reg_string.data, reg_string.size, pattern, pattern_length);
	if (!str) {
		fprintf(stderr, "%s is not a substring:\n%s\n", pattern,
			reg_string.data);
		return 1;
	}
	if (!include_needle) {
		str = memmem(reg_string.data, reg_string.size, pattern,
			     pattern_length) +
		      pattern_length;
	}
	int substring_length;
	sscanf(str, sscanf_format, &substring->data, &substring_length);
	substring->size = (unsigned long)substring_length;
	return 0;
}

static void str_prepend(const char *first_data, const unsigned long first_size,
			struct string *second)
{
	second->data = realloc(second->data, second->size + first_size + 1);
	memmove(second->data + first_size, second->data, second->size + 1);
	second->size += first_size;
	memcpy(second->data, first_data, first_size);
}

static void get_ses_and_tok_info(const struct string response,
				 struct string *credentials)
{
	free(credentials[0].data);
	extract_string(response, 0, &credentials[0],
		       STR_WITH_STRLEN("<SesInfo>"), "%m[^<]%n");
	str_prepend(STR_WITH_STRLEN("Cookie:"), &credentials[0]);

	free(credentials[1].data);
	extract_string(response, 0, &credentials[1],
		       STR_WITH_STRLEN("<TokInfo>"), "%m[^<]%n");
	str_prepend(STR_WITH_STRLEN("__RequestVerificationToken:"),
		    &credentials[1]);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CURL_EASY_PERFORM(x)                                                   \
	do {                                                                   \
		if (curl_easy_perform(x)) {                                    \
			fputs("Connection error at " __FILE__                  \
			      " line " TOSTRING(__LINE__) ".\n",               \
			      stderr);                                         \
			quick_exit(EXIT_FAILURE);                              \
		}                                                              \
		fputs("Connection succeeded at " __FILE__                      \
		      " line " TOSTRING(__LINE__) ".\n",                       \
		      stderr);                                                 \
	} while (0)

static void get_initial_credentials(struct string *credentials)
{
	CURL *curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, ROUTER_URL "webserver/SesTokInfo");
	struct string response = {.data = malloc(1), .size = 0};
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	CURL_EASY_PERFORM(curl);
	curl_easy_cleanup(curl);

	get_ses_and_tok_info(response, credentials);

	free(response.data);

	save_to_file(credentials[0], SES_INFO_0_FILE);
	save_to_file(credentials[1], TOK_INFO_0_FILE);
}

static void unprivileged_request(struct string *credentials,
				 const char *api_url, struct string *response)
{
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, api_url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, credentials[0].data);
	// headers = curl_slist_append(headers, credentials[1].data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	CURL_EASY_PERFORM(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void send_unprivileged_request(struct string *credentials,
				      const char *api_url,
				      struct string *substring,
				      const char *pattern,
				      const unsigned long pattern_length,
				      struct string *response)
{
	unprivileged_request(credentials, api_url, response);
	if (extract_string(*response, 0, substring, pattern, pattern_length,
			   "%m[^<]%n")) {
		fputs("Refreshing credentials.\n", stderr);
		get_initial_credentials(credentials);
		response->size = 0;
		unprivileged_request(credentials, api_url, response);
		extract_string(*response, 0, substring, pattern, pattern_length,
			       "%m[^<]%n");
	}
}

static void extract_and_convert_net_load_rate(struct string temp,
					      struct number *net_load_rate)
{
	net_load_rate->value = strtod(temp.data, 0);
	free(temp.data);

	if (net_load_rate->value < 1024) {
		net_load_rate->unit = "B/s";
	} else if (net_load_rate->value < 1048576) {
		net_load_rate->value /= 1024;
		net_load_rate->unit = "KB/s";
	} else if (net_load_rate->value < 1073741824) {
		net_load_rate->value /= 1048576;
		net_load_rate->unit = "MB/s";
	} else {
		net_load_rate->value /= 1073741824;
		net_load_rate->unit = "GB/s";
	}
}

static void monitoring_traffic_statistics(struct string *credentials,
					  struct number *net_load_rate)
{
	struct string curent_download_rate;
	struct string traffic_statistics = {.data = malloc(1), .size = 0};
	send_unprivileged_request(
	    credentials, ROUTER_URL "monitoring/traffic-statistics",
	    &curent_download_rate, STR_WITH_STRLEN("<CurrentDownloadRate>"),
	    &traffic_statistics);

	extract_and_convert_net_load_rate(curent_download_rate,
					  &net_load_rate[0]);

	struct string curent_upload_rate;
	extract_string(traffic_statistics, 0, &curent_upload_rate,
		       STR_WITH_STRLEN("<CurrentUploadRate>"), "%m[^<]%n");
	free(traffic_statistics.data);
	extract_and_convert_net_load_rate(curent_upload_rate,
					  &net_load_rate[1]);
}

static void monitoring_status(struct string *credentials,
			      unsigned char *connected_users)
{
	struct string current_wifi_users;
	struct string status = {.data = malloc(1), .size = 0};
	send_unprivileged_request(
	    credentials, ROUTER_URL "monitoring/status", &current_wifi_users,
	    STR_WITH_STRLEN("<CurrentWifiUser>"), &status);
	sscanf(current_wifi_users.data, "%hhu", connected_users);
	free(current_wifi_users.data);
	free(status.data);
}

static void print_info(struct string *credentials)
{
	unsigned char connected_users;
	monitoring_status(credentials, &connected_users);

	struct number net_load_rate[2];
	monitoring_traffic_statistics(credentials, net_load_rate);
	printf("%hhu | %.2f %s | %.2f %s\n", connected_users,
	       net_load_rate[0].value, net_load_rate[0].unit,
	       net_load_rate[1].value, net_load_rate[1].unit);
}

static void challenge_login(const struct string *credentials,
			    const struct string *scram,
			    struct string *header_response,
			    struct string *request_response)
{
	CURL *curl = curl_easy_init();
	struct curl_slist *headers = 0;

	headers = curl_slist_append(headers, credentials[0].data);
	headers = curl_slist_append(headers, credentials[1].data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_URL, ROUTER_URL "user/challenge_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
			 "<request><username>" USERNAME
			 "</username><firstnonce>" FIRSTNONCE
			 "</firstnonce></request>");

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, request_response);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, get_response_data);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_response);

	CURL_EASY_PERFORM(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void get_scram_and_update_tok_info(struct string *credentials,
					  struct string *scram)
{
	struct string header_response = {.data = malloc(1), .size = 0};
	struct string request_response = {.data = malloc(1), .size = 0};

	challenge_login(credentials, scram, &header_response,
			&request_response);

	if (extract_string(request_response, 0, &scram[0],
			   STR_WITH_STRLEN("<salt>"), "%m[^<]%n")) {
		get_initial_credentials(credentials);
		header_response.size = 0;
		request_response.size = 0;

		puts(credentials[0].data);
		puts(credentials[1].data);
		challenge_login(credentials, scram, &header_response,
				&request_response);
		extract_string(request_response, 0, &scram[0],
			       STR_WITH_STRLEN("<salt>"), "%m[^<]%n");
	}

	extract_string(request_response, 0, &scram[1],
		       STR_WITH_STRLEN("<servernonce>"), "%m[^<]%n");
	extract_string(request_response, 0, &scram[2],
		       STR_WITH_STRLEN("<iterations>"), "%m[^<]%n");
	free(request_response.data);

	free(credentials[1].data);
	extract_string(header_response, 1, &credentials[1],
		       STR_WITH_STRLEN("__RequestVerificationToken:"),
		       "%m[^\r\n]%n");
	free(header_response.data);
}

static void
get_authentication_login_data(const struct string *scram,
			      struct string *authentication_login_data)
{
	unsigned char salt_bin[SHA256_DIGEST_LENGTH];
	sscanf(scram[0].data, BIN_HEX_FORMAT, REPEAT_32_TIMES(&salt_bin));
	free(scram[0].data);

	unsigned char salted_password[SHA256_DIGEST_LENGTH];
	char password[] = PASSWORD;
	PKCS5_PBKDF2_HMAC(password, STRLEN(password), salt_bin,
			  SHA256_DIGEST_LENGTH, atoi(scram[2].data),
			  EVP_sha256(), SHA256_DIGEST_LENGTH, salted_password);
	free(scram[2].data);

	char key[] = "Client Key";
	unsigned char client_key[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), key, STRLEN(key), salted_password,
	     SHA256_DIGEST_LENGTH, client_key, 0);

	unsigned char storedkey[SHA256_DIGEST_LENGTH];
	SHA256(client_key, SHA256_DIGEST_LENGTH, storedkey);

	struct string auth_msg;
	auth_msg.size = 2 + (scram[1].size << 1) + STRLEN(FIRSTNONCE);
	auth_msg.data = malloc(auth_msg.size + 1);
	snprintf(auth_msg.data, auth_msg.size + 1, "%s,%s,%s", FIRSTNONCE,
		 scram[1].data, scram[1].data);

	unsigned char signature[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), auth_msg.data, auth_msg.size, storedkey,
	     SHA256_DIGEST_LENGTH, signature, 0);
	free(auth_msg.data);

	for (unsigned char i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		client_key[i] ^= signature[i];
	}

	authentication_login_data->size =
	    asprintf(&authentication_login_data->data,
		     "<request>"
		     "<clientproof>" BIN_HEX_FORMAT "</clientproof>"
		     "<finalnonce>%s</finalnonce>"
		     "</request>",
		     REPEAT_32_TIMES(client_key), scram[1].data);
	free(scram[1].data);
}

static void authentication_login(const struct string *credentials,
				 const struct string authentication_login_data,
				 struct string *header_response)
{
	CURL *curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL,
			 ROUTER_URL "user/authentication_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, authentication_login_data);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, credentials[0].data);
	headers = curl_slist_append(headers, credentials[1].data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, get_response_data);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_response);

	// curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

	CURL_EASY_PERFORM(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(authentication_login_data.data);
}

static void update_credentials(const struct string header_response,
			       struct string *credentials)
{
	free(credentials[0].data);
	extract_string(header_response, 1, &credentials[0],
		       STR_WITH_STRLEN("Cookie:"), "%m[^;]%n");
	free(credentials[1].data);
	extract_string(header_response, 0, &credentials[1],
		       STR_WITH_STRLEN("__RequestVerificationTokenone:"),
		       "%m[^\r\n]%n");
	str_prepend(STR_WITH_STRLEN("__RequestVerificationToken:"),
		    &credentials[1]);

	save_to_file(credentials[0], SES_INFO_0_FILE);
	save_to_file(credentials[1], TOK_INFO_0_FILE);

	save_to_file(credentials[0], SES_INFO_1_FILE);
	save_to_file(credentials[1], TOK_INFO_1_FILE);
}

static void elevate_session(struct string *elevated_credentials,
			    struct string *credentials)
{
	struct string scram[3];

	get_scram_and_update_tok_info(credentials, scram);

	struct string authentication_login_data;
	get_authentication_login_data(scram, &authentication_login_data);
	struct string header_response = {.data = malloc(1), .size = 0};
	authentication_login(credentials, authentication_login_data,
			     &header_response);

	update_credentials(header_response, elevated_credentials);

	free(header_response.data);
}

// static void send_privileged_request(
static void privileged_request(struct string *elevated_credentials,
			       const char *api_url, const char *api_data,
			       struct string *response)
{
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, api_url);
	if (api_data) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, api_data);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, elevated_credentials[0].data);
	headers = curl_slist_append(headers, elevated_credentials[1].data);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	CURL_EASY_PERFORM(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void send_privileged_request(struct string *credentials,
				    struct string *elevated_credentials,
				    const char *api_url, const char *api_data,
				    struct string *substring,
				    const char *pattern,
				    const unsigned long pattern_length)
{
	struct string response = {.data = malloc(1), .size = 0};
	privileged_request(elevated_credentials, api_url, api_data, &response);
	puts(response.data);

	if (extract_string(response, 0, substring, pattern, pattern_length,
			   "%m[^<]%n")) {
		elevate_session(elevated_credentials, credentials);
		response.size = 0;
		privileged_request(elevated_credentials, api_url, api_data,
				   &response);
		extract_string(response, 0, substring, pattern, pattern_length,
			       "%m[^<]%n");
	}
	free(response.data);
}

static void reboot(struct string *credentials,
		   struct string *elevated_credentials)
{
	struct string response;
	send_privileged_request(credentials, elevated_credentials,
				ROUTER_URL "device/control",
				"<request><Control>1</Control></request>",
				&response, STR_WITH_STRLEN("<response>"));
	puts(response.data);
}

// 108007 means you overdid it.
int main()
{
	struct string credentials[2];
	retrieve_from_file(&credentials[0], SES_INFO_0_FILE);
	retrieve_from_file(&credentials[1], TOK_INFO_0_FILE);

	struct string elevated_credentials[2];
	retrieve_from_file(&elevated_credentials[0], SES_INFO_1_FILE);
	retrieve_from_file(&elevated_credentials[1], TOK_INFO_1_FILE);

	// print_info(credentials);

	return 0;

	// elevate_session(elevated_credentials, credentials);
	// return 0;

	// reboot(credentials, elevated_credentials);

	struct string response;
	send_privileged_request(credentials, elevated_credentials,
				ROUTER_URL "lan/HostInfo", 0, &response,
				STR_WITH_STRLEN("<message>"));

	send_privileged_request(credentials, elevated_credentials,
				ROUTER_URL "wlan/multi-macfilter-settings-ex",
				0, &response, STR_WITH_STRLEN("<message>"));
	// reboot(credentials, elevated_credentials);

	// get_initial_credentials(credentials);

	// for (unsigned short seconds = 0;; ++seconds) {
	// 	print_info(credentials);
	// 	fprintf(stderr, "%d\n", seconds);
	// 	my_sleep(1);
	// }

	free(credentials[0].data);
	free(credentials[1].data);
}
