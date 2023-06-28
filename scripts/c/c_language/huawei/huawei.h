#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif

#include "regex.h"
#include <curl/curl.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/****************************************************************************/
#define ROUTER_URL "http://192.168.8.1/api/"
#define USERNAME "admin"
#define PASSWORD "SS33550336"
/****************************************************************************/

#define PRIVILEGED_SESSION_FILE "/tmp/privileged_session"
#define UNPRIVILEGED_SESSION_FILE "/tmp/unprivileged_session"

#define DEBUG 0
#include <preprocessor_macros.h>

#define DECIMAL_BASE 10U

#define FIRSTNONCE "01234567890123456789012345678901234567890123456789012345"\
	"67890123"

static unsigned char
extract_substring(char *pattern, char *string, char **substring)
{
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, string, 2, matches, 0)) {
		regfree(&preg);
		return 1;
	}
	unsigned long match_length =
		(unsigned long)(matches[1].rm_eo - matches[1].rm_so);

	if (!*substring) {
		*substring = malloc(match_length + 1);
	} else {
		*substring = realloc(*substring, match_length + 1);
	}
	memcpy(*substring, string + matches[1].rm_so, match_length);
	*(*substring + match_length) = 0;
	regfree(&preg);
	return 0;
}

static unsigned short fetch_ses_and_tok_info(char *ptr, unsigned short size,
	unsigned short nmemb, char **credentials)
{
	char *ses_info = 0;
	extract_substring("<SesInfo>([^<]+)", ptr, &ses_info);
	if (credentials[0]) {
		free(credentials[0]);
	}
	asprintf(&credentials[0], "Cookie:%s",
		ses_info);
	free(ses_info);

	char *tok_info = 0;
	extract_substring("<TokInfo>([^<]+)", ptr, &tok_info);

	if (credentials[1]) {
		free(credentials[1]);
	}
	asprintf(&credentials[1],
		"__RequestVerificationToken:%s", tok_info);
	free(tok_info);
	return size * nmemb;
}

static void save_credentials(char *file_path, char **credentials)
{
	TRACE("save: %s in %s\n", credentials[2], file_path)
	TRACE("save: %s in %s\n", credentials[1], file_path)
	FILE *session_info = fopen(file_path, "we");
	fprintf(session_info, "%s\n%s\n", credentials[0], credentials[1]);
	fclose(session_info);
}

static unsigned char retrieve_credentials(char *file_path, char **credentials)
{
	FILE *session_info = fopen(file_path, "re");
	if (!session_info) {
		return 1;
	}
	fscanf(session_info, "%m[^\n]\n%m[^\n]",
		&credentials[0], &credentials[1]);
	fclose(session_info);
	TRACE("retrieve: %s in %s\n", credentials[0], file_path)
	TRACE("retrieve: %s in %s\n", credentials[1], file_path)
	return 0;
}

static void get_initial_credentials(char **credentials)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			ROUTER_URL "webserver/SesTokInfo");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, credentials);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
		fetch_ses_and_tok_info);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	save_credentials(UNPRIVILEGED_SESSION_FILE, credentials);
}

static unsigned short grab_output(const char *ptr, unsigned short size,
	unsigned short nmemb, char **output)
{
	/* Do not fill output if it is NULL */
	if (!output) {
		return size * nmemb;
	}
	if (*output) {
		unsigned long output_size = strlen(*output);
		*output = realloc(*output, nmemb + output_size + 1);
		memcpy(*output + output_size, ptr, nmemb + 1);
	} else {
		*output = malloc(nmemb + 1);
		memcpy(*output, ptr, nmemb + 1);
	}
	return size * nmemb;
}

static unsigned short get_salt_and_servernonce(char *ptr, unsigned short size,
     unsigned short nmemb, char **scram)
{
	extract_substring("<salt>([^<]+)", ptr, &scram[0]);
	extract_substring("<servernonce>([^<]+)", ptr, &scram[1]);
	extract_substring("<iterations>([^<]+)", ptr, &scram[2]);
	return size * nmemb;
}

static unsigned short update_tok_info(char *ptr, unsigned short size,
     unsigned short nmemb, char **tok_info)
{
	extract_substring("(__RequestVerificationToken: ?[[:print:]]+)",
		ptr, tok_info);
	return size * nmemb;
}

static void hex2bin(char *hex_str, unsigned char *raw_binary)
{
	BIGNUM *a= BN_new();
	BN_hex2bn(&a, hex_str);
	BN_bn2bin(a, raw_binary);
	BN_free(a);
}

static unsigned short update_credentials(char *ptr, unsigned short size,
	unsigned short nmemb, char **credentials)
{
	extract_substring("Set-(Cookie[^;]+)", ptr, &credentials[0]);
	if (strstr(ptr, "__RequestVerificationTokenone:")) {
		struct re_pattern_buffer preg;
		regcomp(&preg,
			"__RequestVerificationTokenone: ?([[:print:]]+)",
			REG_EXTENDED);
		regmatch_t matches[2];
		if (regexec(&preg, ptr, 2, matches, 0)) {
			quick_exit(1);
		}
		if (credentials[1]) {
			free(credentials[1]);
		}
		asprintf(&credentials[1],
			"__RequestVerificationToken:%.*s",
			matches[1].rm_eo - matches[1].rm_so,
			ptr + matches[1].rm_so);
		regfree(&preg);
	}
	return size * nmemb;
}


static void challenge_login(char **credentials, char **scram)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}
	struct curl_slist *headers = 0;

	headers = curl_slist_append(headers, credentials[0]);
	headers = curl_slist_append(headers, credentials[1]);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_URL,
			ROUTER_URL "user/challenge_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
	"<request><username>" USERNAME "</username><firstnonce>" FIRSTNONCE
	"</firstnonce></request>");

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_salt_and_servernonce);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, scram);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, update_tok_info);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &credentials[1]);

	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

static void elevate_session(char **credentials)
{
	/*
	* Huawei router does not allow previously used credentials, therefore
	* the unprivileged credentials must be refreshed. Trust me,
	* I really tried.
	*/
	get_initial_credentials(credentials);

	char *scram[3] = {0};
	challenge_login(credentials, scram);

	unsigned char salt_bin[SHA256_DIGEST_LENGTH];
	hex2bin(scram[0], salt_bin);
	free(scram[0]);

	unsigned char salted_password[SHA256_DIGEST_LENGTH];

	char password[] = PASSWORD;
	PKCS5_PBKDF2_HMAC(password, sizeof(password) - 1, salt_bin,
		SHA256_DIGEST_LENGTH,
		atoi(scram[2]),
		EVP_sha256(),
		SHA256_DIGEST_LENGTH, salted_password);
	free(scram[2]);

	char key[] = "Client Key";
	unsigned char client_key[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), key, sizeof(key) - 1,
		salted_password, SHA256_DIGEST_LENGTH, client_key, 0);

	unsigned char storedkey[SHA256_DIGEST_LENGTH];
	/* SHA256_CTX ctx; */
	/* SHA256_Init(&ctx); */
	/* SHA256_Update(&ctx, client_key, SHA256_DIGEST_LENGTH); */
	/* SHA256_Final(storedkey, &ctx); */

	SHA256(client_key, SHA256_DIGEST_LENGTH, storedkey);

	char *auth_msg = 0;
	int auth_msg_str_len = asprintf(&auth_msg, "%s,%s,%s",
		FIRSTNONCE, scram[1], scram[1]);
	unsigned char signature[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), auth_msg, auth_msg_str_len, storedkey,
		SHA256_DIGEST_LENGTH, signature, 0);
	free(auth_msg);

	for (unsigned char i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		*(client_key + i) ^= *(signature + i);
	}

	BIGNUM *b= BN_new();
	BN_bin2bn(client_key, SHA256_DIGEST_LENGTH, b);
	char *clientproof= BN_bn2hex(b);
	char *authentication_login_data = 0;
	asprintf(&authentication_login_data, "<request>"
		"<clientproof>%s</clientproof>"
		"<finalnonce>%s</finalnonce>"
		"</request>", clientproof, scram[1]);
	OPENSSL_free(clientproof);
	BN_free(b);
	free(scram[1]);

	CURL *curl = curl_easy_init();
	if (!curl) {
		quick_exit(1);
	}

	curl_easy_setopt(curl, CURLOPT_URL,
		ROUTER_URL "user/authentication_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, authentication_login_data);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, credentials[0]);
	headers = curl_slist_append(headers, credentials[1]);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, update_credentials);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, credentials);

	/* Redirect the response to /dev/null since there is no use for it. */
	FILE *dev_null = fopen("/dev/null", "w+e");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, dev_null);
	/* Do not position curl_easy_perform() before fclose(). */
	curl_easy_perform(curl);
	fclose(dev_null);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	free(authentication_login_data);

	save_credentials(PRIVILEGED_SESSION_FILE, credentials);
}


static unsigned char run_request(char **credentials,
	char *api_url, char *api_data, char **response)
{
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, api_url);

	if (api_data) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, api_data);
	}

	/*
	* Make sure response is not provided as NULL and then check whether
	* response is previously malloc'd.
	*/
	if (response && *response) {
		free(*response);
		*response = 0;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, grab_output);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, credentials[0]);
	headers = curl_slist_append(headers, credentials[1]);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return response && strstr(*response, "<code>12500") ? 1 : 0;
}

static void send_request(unsigned char refresh_credentials,
	unsigned char requires_authentication, char *api_url,
	char *api_data, char **response)
{
	char *credentials[2] = {0};

	if (refresh_credentials) {
		if (requires_authentication) {
			elevate_session(credentials);
		} else {
			get_initial_credentials(credentials);
		}
	} else if (requires_authentication &&
			retrieve_credentials(PRIVILEGED_SESSION_FILE,
			credentials)) {
		TRACE("File not found\n")
		elevate_session(credentials);
	} else if (!requires_authentication &&
			retrieve_credentials(UNPRIVILEGED_SESSION_FILE,
			credentials)) {
		TRACE("File not found\n")
		get_initial_credentials(credentials);
	}

	if (run_request(credentials, api_url, api_data, response)) {
		TRACE("Refreshing tokens!\n")
		if (requires_authentication) {
			elevate_session(credentials);
		} else {
			get_initial_credentials(credentials);
		}
		run_request(credentials, api_url, api_data, response);
	}
	free(credentials[0]);
	free(credentials[1]);
}
