#include <curl/curl.h>
#include <openssl/ssl.h>
#include "regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_LENGTH 61
#define COOKIE_LENGTH 147

#define PRIVILEGED_SESSION_FILE "/tmp/privileged_session"
#define UNPRIVILEGED_SESSION_FILE "/tmp/unprivileged_session"

struct credentials {
	char tok_info[TOKEN_LENGTH];
	char ses_info[COOKIE_LENGTH];
};

struct new_credentials {
	char *new_tok_info;
	char *new_ses_info;
};

static void extract_substring(char *pattern, char *string, char **substring)
{
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	if (regexec(&preg, string, 2, matches, 0)) {
		quick_exit(1);
	}
	unsigned long match_length =
		(unsigned long)(matches[1].rm_eo - matches[1].rm_so);

	if (!*substring) {
		*substring = malloc(match_length + 1);
	}
	memcpy(*substring, string + matches[1].rm_so, match_length);
	*(*substring + match_length) = 0;
	regfree(&preg);
}

static void extract_string(const char *str, unsigned short idx,
	const char *start, const char *end, char *substring)
{
	for (unsigned short i = 0, j = 0, k = 0; str[i]; ++i) {
		if (!start[j]) {
			if (str[i] != end[k++]) {
				k = 0;
			} else if (!end[k]) {
				substring[idx - k + 1] = 0;
				break;
			}
			substring[idx++] = str[i];
		} else if (str[i] != start[j++]) {
			j = 0;
		}
	}
}

static unsigned short update_tok_info(char *ptr, unsigned short size,
	unsigned short nmemb, char *token)
{
	if (strstr(ptr, "__RequestVerificationToken:")) {
		unsigned char i = 0;
		for (; ptr[i + 2]; ++i) {
			token[i] = ptr[i];
		}
		token[i] = 0;
	}
	return size * nmemb;
}

static unsigned short update_credentials(char *ptr, unsigned short size,
	unsigned short nmemb, struct credentials *userdata)
{
	if (strstr(ptr, "__RequestVerificationTokenone:")) {
		unsigned char i = 0;
		for (char *temp_string = "__RequestVerificationToken:";
			temp_string[i]; ++i) {
			userdata->tok_info[i] = temp_string[i];
		}
		for (; ptr[i + 5]; ++i) {
			userdata->tok_info[i] = ptr[i + 3];
		}
		userdata->tok_info[i] = 0;
	} else if (strstr(ptr, "Set-Cookie:")) {
		unsigned char i = 0;
		for (unsigned char j = 4; ptr[j] != ';'; ++i, ++j) {
			userdata->ses_info[i] = ptr[j];
		}
		userdata->ses_info[i] = 0;
	}
	return size * nmemb;
}

#define SALT_LENGTH 65
#define SERVERNONCE_LENGTH 97
struct challenge_login_response {
	char salt[SALT_LENGTH];
	char servernonce[SERVERNONCE_LENGTH];
};

static unsigned short get_salt_and_servernonce(char *ptr, unsigned short size,
	unsigned short nmemb, struct challenge_login_response *userdata)
{
	extract_string(ptr, 0, "lt>", "<", userdata->salt);
	extract_string(ptr, 0, "ce>", "<", userdata->servernonce);

	return size * nmemb;
}

static void hex2bin(char *hex_str, unsigned char *raw_binary)
{
	BIGNUM *a= BN_new();
	BN_hex2bn(&a, hex_str);
	BN_bn2bin(a, raw_binary);
	BN_free(a);
}

static void save_credentials(char *file_path,
	struct credentials *logged_in_session)
{
	FILE *session_info = fopen(file_path, "we");
	fprintf(session_info, "%s\n%s\n", logged_in_session->ses_info,
		logged_in_session->tok_info);
	fclose(session_info);
}

#define ITERATION 100
static void salt_the_password(unsigned char *salt,
	unsigned char *encrypted_password)
{
	char password[] = PASSWORD;
	PKCS5_PBKDF2_HMAC(password, sizeof(password) - 1, salt,
		SHA256_DIGEST_LENGTH, ITERATION, EVP_sha256(),
		SHA256_DIGEST_LENGTH, encrypted_password);
}

static void authenticate_login(char *final_post_data,
	struct credentials *unprivileged_session)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL,
		ROUTER_URL "api/user/authentication_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, final_post_data);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, unprivileged_session->tok_info);
	headers = curl_slist_append(headers, unprivileged_session->ses_info);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, update_credentials);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, unprivileged_session);

	/* Redirect the response to /dev/null since there is no use for it. */
	FILE *dev_null = fopen("/dev/null", "w+e");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, dev_null);
	/* Do not position curl_easy_perform() before fclose(). */
	curl_easy_perform(curl);
	fclose(dev_null);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

#define FIRSTNONCE "01234567890123456789012345678901234567890123456789012345\
67890123"
static void elevate_session(struct credentials *unprivileged_session)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		return;
	}

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, unprivileged_session->ses_info);
	headers = curl_slist_append(headers, unprivileged_session->tok_info);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_URL,
			ROUTER_URL "api/user/challenge_login");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
	"<request><username>" USERNAME "</username><firstnonce>" FIRSTNONCE
	"</firstnonce></request>");

	struct challenge_login_response scram;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
		get_salt_and_servernonce);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &scram);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, update_tok_info);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA,
		unprivileged_session->tok_info);
	curl_easy_perform(curl);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	unsigned char salt_bin[SHA256_DIGEST_LENGTH];
	hex2bin(scram.salt, salt_bin);

	unsigned char salted_password[SHA256_DIGEST_LENGTH];
	salt_the_password(salt_bin, salted_password);

	char key[] = "Client Key";
	unsigned char client_key[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), key, sizeof(key) - 1,
		salted_password, SHA256_DIGEST_LENGTH, client_key, 0);

	unsigned char storedkey[SHA256_DIGEST_LENGTH];
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, client_key, SHA256_DIGEST_LENGTH);
	SHA256_Final(storedkey, &ctx);

	char *auth_msg = 0;
	asprintf(&auth_msg, "%s,%s,%s", FIRSTNONCE, scram.servernonce,
		scram.servernonce);
	unsigned char signature[SHA256_DIGEST_LENGTH];
	HMAC(EVP_sha256(), auth_msg, (int)strlen(auth_msg), storedkey,
		SHA256_DIGEST_LENGTH, signature, 0);
	free(auth_msg);

	for (unsigned char i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		client_key[i] = client_key[i] ^ signature[i];
	}

	BIGNUM *b= BN_new();
	BN_bin2bn(client_key, SHA256_DIGEST_LENGTH, b);
	char *clientproof= BN_bn2hex(b);
	char *authentication_login_data = 0;
	asprintf(&authentication_login_data, "<request>"
		"<clientproof>%s</clientproof>"
		"<finalnonce>%s</finalnonce>"
		"</request>", clientproof, scram.servernonce);
	OPENSSL_free(clientproof);
	BN_free(b);

	authenticate_login(authentication_login_data, unprivileged_session);
	free(authentication_login_data);

	save_credentials(PRIVILEGED_SESSION_FILE, unprivileged_session);
}

static unsigned short fetch_ses_and_tok_info(char *ptr, unsigned short size,
	unsigned short nmemb, struct credentials *userdata)
{
	printf("%s\n", ptr);
#if 1
	struct new_credentials new_current_session;

	char *ses_info = 0;
	extract_substring("<SesInfo>([^<]+)", ptr, &ses_info);
	asprintf(&new_current_session.new_ses_info, "Cookie:%s", ses_info);
	free(ses_info);
	printf("%s\n", new_current_session.new_ses_info);
	free(new_current_session.new_ses_info);

	char *tok_info = 0;
	extract_substring("<TokInfo>([^<]+)", ptr, &tok_info);
	asprintf(&new_current_session.new_tok_info,
		"__RequestVerificationToken:%s", tok_info);
	printf("%s\n", new_current_session.new_tok_info);
	free(tok_info);
	free(new_current_session.new_tok_info);
#endif

	unsigned short i = 0;
	for (char *temp_string = "Cookie:";
		temp_string[i]; ++i) {
		userdata->ses_info[i] = temp_string[i];
	}
	extract_string(ptr, i, "sInfo>", "<", userdata->ses_info);

	i = 0;
	for (char *temp_string = "__RequestVerificationToken:";
		temp_string[i]; ++i) {
		userdata->tok_info[i] = temp_string[i];
	}
	extract_string(ptr, i, "kInfo>", "<", userdata->tok_info);
	return size * nmemb;
}

static void get_initial_credentials(struct credentials *current_session)
{
	CURL *curl = curl_easy_init();
	if (!curl) {
		return;
	}
	curl_easy_setopt(curl, CURLOPT_URL,
			ROUTER_URL "api/webserver/SesTokInfo");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, current_session);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_ses_and_tok_info);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	save_credentials(UNPRIVILEGED_SESSION_FILE, current_session);
}

static unsigned short grab_output(const char *ptr, unsigned short size,
	unsigned short nmemb, char **output)
{
	*output = malloc(nmemb + 1);

	/* memcpy is faster than strcpy. */
	memcpy(*output, ptr, nmemb + 1);
	return size * nmemb;
}

static void read_until(char *string, FILE *file, char character) {
	for (int c = fgetc(file), i = 0;; c = fgetc(file), ++i) {
		if (c == character) {
			string[i] = 0;
			return;
		}
		string[i] = (char)c;
	}
}

static void retrieve_credentials(unsigned char requires_authentication,
	char *file_path, struct credentials *current_session)
{
	FILE *session_info = fopen(file_path, "re");

	/* In case the file does not exist. */
	if (!session_info) {
		get_initial_credentials(current_session);
		if (requires_authentication) {
			elevate_session(current_session);
		}
		return;
	}
	read_until(current_session->ses_info, session_info, '\n');
	read_until(current_session->tok_info, session_info, '\n');
	fclose(session_info);
}

static void send_request(unsigned char refresh_credentials,
	unsigned char requires_authentication,
	char *api_url, char *api_data, char **response)
{
	struct credentials current_session;

	if (refresh_credentials) {
		label:
		get_initial_credentials(&current_session);
	} else {
		if (!requires_authentication) {
			retrieve_credentials(requires_authentication,
			UNPRIVILEGED_SESSION_FILE,
			&current_session);
		}
	}

	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, api_url);

	if (api_data) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, api_data);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, grab_output);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

	struct curl_slist *headers = 0;
	headers = curl_slist_append(headers, current_session.tok_info);
	headers = curl_slist_append(headers, current_session.ses_info);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (strstr(*response, ">12500") && !refresh_credentials ) {
		/* Discard the error message. */
		**response = 0;

		refresh_credentials = 1;
		goto label;
	}
}
