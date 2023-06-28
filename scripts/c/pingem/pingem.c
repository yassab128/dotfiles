#include "../get_router_ip/get_router_gateway.h"
#include <assert.h>
#include <net/if.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#define INT_TO_ADDR(_addr)                                                     \
	(_addr & 0xFF), (_addr >> 8 & 0xFF), (_addr >> 16 & 0xFF),             \
	    (_addr >> 24 & 0xFF)

#define ADDRNUM 252

static void
ping_it(unsigned int *all_addresses)
{
	ssize_t retval;
	struct icmphdr icmp_hdr;
	struct sockaddr_in addr;

	struct epoll_event events[ADDRNUM];
	struct epoll_event ev = {0};
	int epollfd;

	int sock[ADDRNUM];
	unsigned char i;
	int nfds;

	icmp_hdr.type = ICMP_ECHO;
	icmp_hdr.code = 0;
	icmp_hdr.checksum = 0;
	icmp_hdr.un.echo.id = 0;
	icmp_hdr.un.echo.sequence = 0;

	addr.sin_family = AF_INET;
	addr.sin_port = 0;

	epollfd = epoll_create1(0);
	assert(epollfd != -1);

	for (i = 0; i < ADDRNUM; ++i) {
		sock[i] =
		    socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_ICMP);
		assert(sock[i] != -1);

		addr.sin_addr.s_addr = all_addresses[i];
		retval = sendto(sock[i], &icmp_hdr, sizeof(icmp_hdr), 0,
		    (struct sockaddr *)&addr, sizeof(addr));
		assert(retval != -1);

		ev.events = EPOLLIN;
		ev.data.u32 = i;
		retval = epoll_ctl(epollfd, EPOLL_CTL_ADD, sock[i], &ev);
		assert(retval != -1);
	}
	sleep(1);
	nfds = epoll_wait(epollfd, events, ADDRNUM, 0);
	assert(nfds != -1);

	for (i = 0; i < nfds; ++i) {
		printf("%d.%d.%d.%d\n",
		    INT_TO_ADDR(all_addresses[events[i].data.u32]));
	}
	retval = close(epollfd);
	assert(retval != -1);

	for (i = 0; i < ADDRNUM; ++i) {
		retval = close(sock[i]);
		assert(retval != -1);
	}
}

static unsigned int
get_ip_address()
{
	ssize_t retval;
	struct ifconf ifc;
	struct ifreq ifr[32];
	int sd;
	int ifc_num;
	int i;
	unsigned int ip_address = 0;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(sd != -1);
	ifc.ifc_len = sizeof(ifr);
	ifc.ifc_buf = (char *)ifr;

	retval = ioctl(sd, SIOCGIFCONF, &ifc);
	assert(!retval);

	ifc_num = ifc.ifc_len / (int)sizeof(struct ifreq);
	for (i = 0; i < ifc_num; ++i) {
		if (ioctl(sd, SIOCGIFFLAGS, &ifr[i])) {
			continue;
		}
		if (ifr[i].ifr_flags & IFF_LOOPBACK ||
		    !(ifr[i].ifr_flags & IFF_BROADCAST)) {
			continue;
		}
		if (ioctl(sd, SIOCGIFADDR, &ifr[i])) {
			continue;
		}
		ip_address =
		    ((struct sockaddr_in *)(&ifr[i].ifr_addr))->sin_addr.s_addr;
	}
	retval = close(sd);
	assert(retval != -1);

	return ip_address;
}

static unsigned char
get_gateway_fourth_byte()
{
	unsigned char *bytes;
	unsigned int gateway;

	gateway = get_gateway();
	bytes = (unsigned char *)&gateway;
	printf("%d.%d.%d.%d (gateway)\n", INT_TO_ADDR(gateway));
	return bytes[3];
}

static void
get_all_network_addresses(unsigned int *all_addresses)
{
	unsigned int ip_address;
	unsigned char i;
	unsigned char n = 0;
	unsigned char j;
	unsigned char k;
	unsigned char *bytes;
	unsigned char ip_address_fourth_byte;
	unsigned char gateway_fourth_byte;

	gateway_fourth_byte = get_gateway_fourth_byte();

	ip_address = get_ip_address();
	printf("%d.%d.%d.%d (address)\n", INT_TO_ADDR(ip_address));
	bytes = (unsigned char *)&ip_address;
	ip_address_fourth_byte = bytes[3];

	if (gateway_fourth_byte < ip_address_fourth_byte) {
		j = gateway_fourth_byte;
		k = ip_address_fourth_byte;
	} else {
		j = ip_address_fourth_byte;
		k = gateway_fourth_byte;
	}
	for (i = 1; i < j; ++i) {
		bytes[3] = i;
		all_addresses[n++] = ip_address;
	}
	++i;
	for (; i < k; ++i) {
		bytes[3] = i;
		all_addresses[n++] = ip_address;
	}
	++i;
	for (; i < 0xFF; ++i) {
		bytes[3] = i;
		all_addresses[n++] = ip_address;
	}
}

int
main()
{
	unsigned int all_addresses[ADDRNUM];
	get_all_network_addresses(all_addresses);
	ping_it(all_addresses);
}
