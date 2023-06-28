#define PORT "80"
#if 0
#	define HOST "192.168.8.7"
#	define RESOURCE "/api/webserver/SesTokInfo"
#else
#	define HOST "example.com"
//#	define HOST "google.com"
#	define RESOURCE "/"
#endif

/* 192.168.8.7/api/webserver/SesTokInfo */

#include "../my_macros.h"
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#define REPLY_TIMEOUT 0

int
main()
{
	int sockfd;
	struct addrinfo *res = 0;
	struct addrinfo hints = {0};
	char request[] = "GET " RESOURCE " HTTP/1.1\r\n"
			 "Host: " HOST "\r\n"
			 "User-Agent: curl/7.81.0\r\n"
			 "Accept: */*\r\n"
			 "Connection: close\r\n"
			 "\r\n";
	char reply[1048576];
	struct pollfd pfds[1];
	long reply_length;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#if 0
	{
		int flags = fcntl(sockfd, F_GETFL, 0);
		if (flags == -1) {
			puts("Nope");
			return 0;
		}
		if (!(flags & O_NONBLOCK)) {
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
		}
	}
#endif
	CHECK(sockfd == -1);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	CHECK(getaddrinfo(HOST, PORT, &hints, &res));
	CHECK(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);
#if 1
	CHECK(write(sockfd, request, sizeof(request) - 1) == -1);
	for (reply_length = read(sockfd, reply, sizeof(reply));
	     reply_length > 0;
	     reply_length = read(sockfd, reply, sizeof(reply))) {
		fwrite(reply, 1, (unsigned long)reply_length, stdout);
	}
#elif 0
	/* Ignore headers */
	pfds->fd = sockfd;
	pfds->events = POLLIN;
	do {
		reply[read(sockfd, reply, sizeof(reply))] = 0;
		fputs(reply, stdout);
		poll(pfds, 1, REPLY_TIMEOUT);
	} while (pfds[0].revents == POLLIN);
#else
#	define SEND_FLAGS MSG_DONTWAIT
#	define RECV_FLAGS 0
	CHECK(send(sockfd, request, sizeof(request), SEND_FLAGS) == -1);
	CHECK(recv(sockfd, reply, sizeof(reply), RECV_FLAGS) == -1);
	CHECK(recv(sockfd, reply, sizeof(reply), RECV_FLAGS) == -1);
#endif
	close(sockfd);
	puts("Success!");
	return 0;
}
