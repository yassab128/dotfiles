#include <assert.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

#define HOST "google.com"
#define RESOURCE "/"

#define ONE_MIB 1048576

static long
https_request(char *buf, char *const host_and_port, const char *const request,
 const int request_length)
{
	BIO *bio;
	SSL_CTX *ctx;
	long retval;
	char *buf_ptr;

	SSL_library_init();

	ctx = SSL_CTX_new(SSLv23_client_method());
	assert(ctx);

	bio = BIO_new_ssl_connect(ctx);
	assert(bio);

	retval = BIO_set_conn_hostname(bio, host_and_port);
	assert(retval);

	retval = BIO_do_connect(bio);
	assert(retval == 1);

	retval = BIO_write(bio, request, request_length);
	assert(retval == request_length);

	buf_ptr = buf;
	for (retval = BIO_read(bio, buf_ptr, ONE_MIB); retval != 0;
	     retval = BIO_read(bio, buf_ptr, ONE_MIB)) {
		buf_ptr += retval;
	}
	BIO_free_all(bio);
	SSL_CTX_free(ctx);

	return buf_ptr - buf;
}

#define BUF_BUFLEN(x) (x), sizeof(x) - 1

int
main()
{
	char buf[ONE_MIB];
	long buflen;

	buflen = https_request(buf, HOST ":443",
	 BUF_BUFLEN("GET " RESOURCE " HTTP/1.1\r\n"
		    "Host: " HOST "\r\n"
		    "Connection: close\r\n"
		    "\r\n"));
	puts(buf);
}
