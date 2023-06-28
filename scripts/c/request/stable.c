#define PORT "80"
#if 1
#	define HOST "192.168.8.7"
#	define RESOURCE "/api/webserver/SesTokInfo"
#elif 0
#	define HOST "majesticastoundingslowyawn.neverssl.com"
#	define RESOURCE "/online"
#elif 0
#	define HOST "192.168.8.7"
#	define RESOURCE "/html/home.html"
#else
#	define HOST "example.com"
#	define RESOURCE "/"
#endif

/* 192.168.8.7/api/webserver/SesTokInfo */

#include "../my_macros.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
get_content_length(const char *const headers, const long headers_length)
{
	return atoi((char *)memmem(headers, (unsigned long)headers_length,
		     STR_WITH_STRLEN("Content-Length: ")) +
	 STRLEN("Content-Length: "));
}

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
	char reply[1048576];
	char headers[1048576];
	char *body;
	long reply_length;
	long offset;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	WARNIF(getaddrinfo(HOST, PORT, &hints, &res));

	sockfd = socket(hints.ai_family, hints.ai_socktype, 0);
	WARNIF(sockfd == -1);

	WARNIF(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);
	WARNIF(write(sockfd, request, sizeof(request) - 1) == -1);
#if 0
	reply_length = read(sockfd, reply, sizeof(reply));
	WARNIF(reply_length == -1);
	offset = (long)(char *)memmem(
		  reply, (unsigned long)reply_length, "\r\n\r\n", 4) -
	 (long)reply;
	reply[offset] = 0;
	memcpy(headers, reply, (unsigned long)offset);
	body = malloc((unsigned long)get_content_length(headers, offset));
	free(body);
	reply_length -= offset + 4;
	if (reply_length) {
		puts("jkflds");
	} else {
	}
	// &reply[offset] = 0;
	printf("%ld | %ld\n", reply_length, offset);
#else
	for (;;) {
		reply_length = read(sockfd, reply, sizeof(reply) - 1);
		if (reply_length < 1) {
			break;
		}
		WARNIF(!fwrite(reply, 1, (unsigned long)reply_length, stdout));
	}
#endif
	WARNIF(close(sockfd));
	return 0;
}
