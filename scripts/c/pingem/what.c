#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <assert.h>
#include <unistd.h>

static void
ping_it(struct in_addr dst)
{
	ssize_t ret;
	struct icmphdr icmp_hdr;
	struct sockaddr_in addr;
	struct pollfd pfds[1];
	int sock;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	assert(sock != -1);

	addr.sin_family = AF_INET;
	addr.sin_addr = dst;
	addr.sin_port = 0;

	icmp_hdr.type = ICMP_ECHO;
	icmp_hdr.code = 0;
	icmp_hdr.checksum = 0;
	icmp_hdr.un.echo.id = 0;
	icmp_hdr.un.echo.sequence = 0;

	ret = sendto(sock, &icmp_hdr, sizeof(icmp_hdr), 0,
	 (struct sockaddr *)&addr, sizeof(addr));
	assert(ret != -1);

	pfds->fd = sock;
	pfds->events = POLLIN;
	switch (poll(pfds, 1, 100)) {
	case 1:
		puts("Yes");
		break;
	case 0:
		puts("No");
		break;
	default:
		perror("poll()");
		return;
	}

	ret = close(sock);
	assert(ret != -1);
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		return 1;
	}
	struct in_addr dst;

	if (inet_aton(argv[1], &dst) == 0) {
		perror("inet_aton");
		printf("%s isn't a valid IP address\n", argv[1]);
		return 1;
	}
	ping_it(dst);
	return 0;
}
