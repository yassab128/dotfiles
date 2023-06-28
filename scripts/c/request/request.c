#define PORT "80"
#if 0
#	define HOST "192.168.8.7"
#	define RESOURCE "/api/webserver/SesTokInfo"
#elif 0
#	define HOST "majesticastoundingslowyawn.neverssl.com"
#	define RESOURCE "/online"
#elif 0
#	define HOST "192.168.8.7"
#	define RESOURCE "/html/home.html"
#else
/*#	define HOST "example.com"
#	define RESOURCE "/"*/
#	define HOST "achamel.info"
#	define RESOURCE "/Lyceens/index.php"
#endif

#include "../my_macros.h"
#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static ssize_t
get_content_nbytes(const char *const headers, const long headers_length)
{
	ssize_t num = strtol((char *)memmem(headers, (size_t)headers_length,
			      STR_WITH_STRLEN("Content-Length: ")) +
			      STRLEN("Content-Length: "),
	 0, 10);
	assert(num);
	return num;
}

#define HALF_MIB 524288

int
main()
{
	int sockfd;
	struct addrinfo *res = 0;
	struct addrinfo hints = {0};
	char request[] = "GET " RESOURCE " HTTP/1.1\r\n"
			 "Host: " HOST "\r\n"
			 "Connection: close\r\n"
			 "\r\n";
	char first_reply[HALF_MIB];
	char *content;
	char *content_ptr;
	char *headers;
	ssize_t reply_nbytes;
	ssize_t headers_nbytes;
	ssize_t content_nbytes;
	ssize_t first_content_nbytes;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	assert(!getaddrinfo(HOST, PORT, &hints, &res));

	sockfd = socket(hints.ai_family, hints.ai_socktype, 0);
	assert(sockfd != -1);

	assert(!connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);

	assert(write(sockfd, request, sizeof(request) - 1) != -1);

	reply_nbytes = read(sockfd, first_reply, sizeof(first_reply));
	assert(reply_nbytes != -1);
	fwrite(first_reply, 1, reply_nbytes, stdout);
	assert(reply_nbytes != -1);
	headers_nbytes = (long)(char *)memmem(
			  first_reply, (size_t)reply_nbytes, "\r\n\r\n", 4) +
			 4 - (long)first_reply;
	content_nbytes = get_content_nbytes(first_reply, headers_nbytes);

	/* The additional 1 is for the NULL terminator. */
	content = malloc((size_t)content_nbytes + 1);
	headers = malloc((size_t)headers_nbytes + 1);

	memcpy(headers, first_reply, (size_t)headers_nbytes);
	headers[headers_nbytes] = 0;

	first_content_nbytes = reply_nbytes - headers_nbytes;
	if (!first_content_nbytes) {
		reply_nbytes =
		 recv(sockfd, content, (size_t)content_nbytes, MSG_WAITALL);
		assert(reply_nbytes == content_nbytes);
	} else if (first_content_nbytes == content_nbytes) {
		memcpy(content, first_reply + headers_nbytes, content_nbytes);
	} else {
		ssize_t content_left_nbytes;
		memcpy(content, first_reply + headers_nbytes,
		 (size_t)first_content_nbytes);
		content_left_nbytes = content_nbytes - first_content_nbytes;
		assert(recv(sockfd, content + first_content_nbytes,
			(size_t)content_left_nbytes,
			MSG_WAITALL) == content_left_nbytes);
	}
	content[content_nbytes] = 0;
	assert(!close(sockfd));
	fputs(headers, stdout);
	fputs(content, stdout);
	free(content);
	free(headers);
	return 0;
}
