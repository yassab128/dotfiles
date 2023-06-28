#include <assert.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

#ifdef curl_easy_setopt
#	undef curl_easy_setopt
#endif

#define GMAIL_CLIENT_ID                                                        \
	"575205653021-ivlgjpvhdm0mp9id16fc3t4irhmfclfk."                       \
	"apps.googleusercontent.com"
#define GMAIL_CLIENT_SECRET "6OslLIoWUNYZ_u2qsydIvSA7"
#define GMAIL_REFRESH_TOKEN                                                    \
	"1//0doIgp17m0RqECgYIARAAGA0SNwF-L9IryYKpd4OExs6"                      \
	"tUj9L9xPbxeDJLUKb9IESudJOHGDeZ6gT__uJEMjwOI_ziXLzSlZeFkw"

#define BUF_BUFLEN(x) (x), sizeof(x) - 1
#define STRLEN(x) sizeof(x) - 1

#define BUFSZ 8192
#define REQUEST_MESSAGE_SIZE 1024

#define REQUEST1_HOST "oauth2.googleapis.com"
#define REQUEST1_RESOURCE "/token"
#define REQUEST1_POST_DATA                                                     \
	"client_id=" GMAIL_CLIENT_ID "&client_secret=" GMAIL_CLIENT_SECRET     \
	"&grant_type=refresh_token&"                                           \
	"refresh_token=" GMAIL_REFRESH_TOKEN
#define REQUEST1_POST_DATA_LENGTH "264" /* strlen(REQUEST1_POST_DATA) */

#define REQUEST2_HOST "gmail.googleapis.com"
#define REQUEST2_RESOURCE                                                      \
	"/gmail/v1/users/me/"                                                  \
	"messages?fields=resultSizeEstimate&q=\"is:unread\""
#define REQUEST2_MESSAGE                                                       \
	"GET " REQUEST2_RESOURCE " HTTP/1.1\r\n"                               \
	"Host: " REQUEST2_HOST "\r\n"                                          \
	"Authorization: Bearer "

#define ACCESS_TOKEN_NEEDLE "access_token\": \""

#define UNREAD_NEEDLE "resultSizeEstimate\": "

static void
check_email()
{
	BIO *bio;
	SSL_CTX *ctx;
	long retval;
	char buf[BUFSZ];
	char message[REQUEST_MESSAGE_SIZE] = REQUEST2_MESSAGE;
	char *message_ptr = message + STRLEN(REQUEST2_MESSAGE);
	char *unread_message_number;

	SSL_library_init();

	ctx = SSL_CTX_new(TLS_client_method());
	assert(ctx);

	bio = BIO_new_ssl_connect(ctx);
	assert(bio);

	retval = BIO_set_conn_hostname(bio, REQUEST1_HOST ":443");
	assert(retval);

	retval = BIO_do_connect(bio);
	assert(retval == 1);

	retval = BIO_write(
	 bio, BUF_BUFLEN("POST " REQUEST1_RESOURCE " HTTP/1.1\r\n"
			 "Host: " REQUEST1_HOST "\r\n"
			 "Content-type: application/x-www-form-urlencoded\r\n"
			 "Content-Length: " REQUEST1_POST_DATA_LENGTH "\r\n"
			 "\r\n" REQUEST1_POST_DATA));
	assert(retval > 0);

	retval = BIO_read(bio, buf, BUFSZ);
	assert(retval > 0);
	{
		char *start;
		char *end;
		start = memmem(
		 buf, (unsigned long)retval, BUF_BUFLEN(ACCESS_TOKEN_NEEDLE));
		assert(start);
		start += STRLEN(ACCESS_TOKEN_NEEDLE);
		end = memchr(start, '"',
				256 /* A number large enough
				     * than strlen(ACCESS_TOKEN)
				     */);
		assert(end);
		end[0] = '\r';
		end[1] = '\n';
		end[2] = '\r';
		end[3] = '\n';
		end[4] = '\0';
		end += 4;
		memcpy(message_ptr, start, (unsigned long)(end - start));
	}
	BIO_reset(bio);
	retval = BIO_set_conn_hostname(bio, REQUEST2_HOST ":443");
	assert(retval);

	retval = BIO_do_connect(bio);
	assert(retval == 1);

	retval = BIO_write(bio, BUF_BUFLEN(message));
	assert(retval > 0);

	retval = BIO_read(bio, buf, BUFSZ);
	assert(retval > 0);

	BIO_free_all(bio);
	SSL_CTX_free(ctx);
	{
		char *end;
		unread_message_number =
		 memmem(buf, (unsigned long)retval, BUF_BUFLEN(UNREAD_NEEDLE));

		assert(unread_message_number);
		unread_message_number += STRLEN(UNREAD_NEEDLE);
		end = memchr(unread_message_number, '\n',
				6 /* You cannot seriously have more than a
				   * million unread messages?
				   */);
		assert(end);
		*end = '\0';
	}
	if (unread_message_number[0] == '0') {
		return;
	}
	puts(unread_message_number);
}

static void
test()
{
	BIO *bio;
	SSL_CTX *ctx;

	SSL_library_init();
	ctx = SSL_CTX_new(TLS_client_method());
	bio = BIO_new_ssl_connect(ctx);
	BIO_set_conn_hostname(bio, REQUEST1_HOST ":443");
	BIO_do_connect(bio);
	BIO_free_all(bio);
	SSL_CTX_free(ctx);
}

/*
int limit_battery()
{

}
*/

int
main()
{
	/* check_email(); */
	test();
}
