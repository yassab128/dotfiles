#define PORT "80"
#if 1
#	define HOST "192.168.8.7"
#	define RESOURCE "/api/webserver/SesTokInfo"
#elif 1
#	define HOST "majesticastoundingslowyawn.neverssl.com"
#	define RESOURCE "/online"
#elif 1
#	define HOST "192.168.8.7"
#	define RESOURCE "/html/home.html"
#elif 1
#	define HOST "example.com"
#	define RESOURCE "/"
#else
#	define HOST "harmful.cat-v.org"
#	define RESOURCE "/software"
#endif

/* 192.168.8.7/api/webserver/SesTokInfo */

#include "../my_macros.h"
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REPLY_LENGTH 1048576

int
main()
{
	int sockfd;
	struct addrinfo *res = 0;
	char request[] = "GET " RESOURCE " HTTP/1.1\n"
			 "Host: " HOST "\n"
			 "Connection: close\n"
			 "\n";
	char reply[REPLY_LENGTH];
	long reply_length;

	WARNIF(getaddrinfo(HOST, PORT, 0, &res));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	WARNIF(sockfd == -1);

	WARNIF(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);

	WARNIF(send(sockfd, request, STRLEN(request), 0) == -1);
	for (;;) {
		reply_length = recv(sockfd, reply, REPLY_LENGTH, 0);
		if (reply_length < 1) {
			break;
		}
		WARNIF(
		 !fwrite(reply, 1, (unsigned long)reply_length, stdout));
		/* puts("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"); */
		/* break; */
	}
	WARNIF(close(sockfd));
	return 0;
}
