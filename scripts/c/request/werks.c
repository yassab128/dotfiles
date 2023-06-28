#define PORT "80"
#if 0
#	define HOST "192.168.8.7"
#	define RESOURCE "/api/webserver/SesTokInfo"
#elif 0
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

int
main()
{
	int sockfd;
	struct addrinfo *res = 0;
	char request[] = "GET " RESOURCE " HTTP/1.1\n"
			 "Host: " HOST "\n"
			 "Connection: close\n"
			 "\n";
	char reply[1048576];
	long reply_length;

	WARNIF(getaddrinfo(HOST, PORT, 0, &res));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	WARNIF(sockfd == -1);

	WARNIF(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);

	WARNIF(send(sockfd, request, sizeof(request), MSG_OOB) == -1);
	for (;;) {
		reply_length = recv(sockfd, reply, sizeof(reply), 0);
		if (reply_length < 1) {
			break;
		}
		WARNIF(!fwrite(reply, 1, (unsigned long)reply_length, stdout));
		puts("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	}
	WARNIF(close(sockfd));
	return 0;
}
