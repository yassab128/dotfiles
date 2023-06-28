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
#else
#	define HOST "example.com"
#	define RESOURCE "/"
#endif

/* majesticastoundingslowyawn.neverssl.com/online */
/* 192.168.8.7/api/webserver/SesTokInfo */

#include "../my_macros.h"
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
	WARNIF(send(sockfd, request, sizeof(request), 0) == -1);
	reply_length = recv(sockfd, reply, sizeof(reply), 0);
*/

static void
get_headers()
{
}

int
main()
{
	int sockfd;
	struct addrinfo *res = 0;
	char request[] = "GET " RESOURCE " HTTP/1.1\r\n"
			 "Host: " HOST "\r\n"
			 "Connection: close\r\n"
			 "\r\n";
	char reply[1048576];
	char headers[1048576];
	long reply_length;

	WARNIF(getaddrinfo(HOST, PORT, 0, &res));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	WARNIF(sockfd == -1);

	WARNIF(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);

	WARNIF(write(sockfd, request, sizeof(request) - 1) == -1);

	// reply_length = read(sockfd, reply, sizeof(reply));
	// WARNIF(reply_length == -1);

#if 0
	for (; 0;) {
		reply_length = read(sockfd, reply, sizeof(reply));
		if (reply_length < 1) {
			break;
		}
		WARNIF(!fwrite(reply, 1, (unsigned long)reply_length, stdout));
		printf("\nxxxxxxxxxxxx%ldxxxxxxxxxxxxxxxx\n", reply_length);
		printf(
		 "\nxxxxxxxxxxxx%dxxxxxxxxxxxxxxxx\n", reply[reply_length - 1]);
	}
#endif
	WARNIF(close(sockfd));
	return 0;
}
