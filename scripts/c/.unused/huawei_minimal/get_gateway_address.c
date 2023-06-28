#include "get_gateway_address.h"
#if 0
void get_it(void)
#endif

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define INT_TO_ADDR(_addr)                                                     \
	(_addr & 0xFF), (_addr >> 8 & 0xFF), (_addr >> 16 & 0xFF),             \
	 (_addr >> 24 & 0xFF)

#define BLYAT 3

#if BLYAT == 1
void
get_it(void)
{
	/* Create a socket so we can use ioctl on the file
	 * descriptor to retrieve the interface info.
	 */

	int sd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr[16];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(ifr);
	ifc.ifc_buf = (caddr_t)ifr;
	char host[300];

	ioctl(sd, SIOCGIFCONF, &ifc);
	int ifc_num = ifc.ifc_len / sizeof(struct ifreq);
	printf("%d interfaces found\n", ifc_num);

	for (int i = 1, hwaddr, network, addr, mask, broadaddr, dstaddr;
	     i < ifc_num; ++i) {
		if (ifr[i].ifr_addr.sa_family != AF_INET) {
			continue;
		}

		printf("%d) interface: %s\n", i + 1, ifr[i].ifr_name);

		// getnameinfo(&ifr[i].ifr_addr, sizeof(struct sockaddr_in),
		// host, 	    sizeof(host), 0, 0, NI_NUMERICHOST);
		// puts(host);

		ioctl(sd, SIOCGIFADDR, &ifr[i]);
		addr =
		 ((struct sockaddr_in *)(&ifr[i].ifr_addr))->sin_addr.s_addr;
		printf("%d) addr: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(addr));

		ioctl(sd, SIOCGIFMETRIC, &ifr[i]);
		printf("%d) metric: %d.%d.%d.%d\n", i + 1,
		 INT_TO_ADDR(ifr[i].ifr_metric));

		ioctl(sd, SIOCGIFHWADDR, &ifr[i]);
		hwaddr =
		 ((struct sockaddr_in *)(&ifr[i].ifr_hwaddr))->sin_addr.s_addr;
		printf("%d) hwaddr: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(hwaddr));

		ioctl(sd, SIOCGIFDSTADDR, &ifr[i]);
		dstaddr =
		 ((struct sockaddr_in *)(&ifr[i].ifr_dstaddr))->sin_addr.s_addr;
		printf(
		 "%d) dstaddr: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(dstaddr));

		ioctl(sd, SIOCGIFBRDADDR, &ifr[i]);
		broadaddr = ((struct sockaddr_in *)(&ifr[i].ifr_broadaddr))
			     ->sin_addr.s_addr;
		printf(
		 "%d) broadaddr: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(broadaddr));
		ioctl(sd, SIOCGIFNETMASK, &ifr[i]);
		mask =
		 ((struct sockaddr_in *)(&ifr[i].ifr_netmask))->sin_addr.s_addr;
		printf("%d) mask: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(mask));

		network = mask & broadaddr;
		printf(
		 "%d) network: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(network));
	}
	close(sd);
}
#elif BLYAT == 2
void
get_it(void)
{
	struct ifconf ifc;
	struct ifreq *ifr;
	int fd;
	int nifs, i;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifc.ifc_len = 0;
	ifc.ifc_ifcu.ifcu_buf = NULL;
	ioctl(fd, SIOCGIFCONF, &ifc);
	printf("%d\n", ifc.ifc_len);

	if (ifc.ifc_len == 0) {
		return;
	}

	ifc.ifc_ifcu.ifcu_buf = (void *)malloc(ifc.ifc_len);
	ioctl(fd, SIOCGIFCONF, &ifc);
	nifs = ifc.ifc_len / sizeof(struct ifreq);
	ifr = (struct ifreq *)ifc.ifc_ifcu.ifcu_buf;

	for (i = 0; i < nifs; i++) {
		printf("%s\n", ifr[i].ifr_name);
	}
}
#elif BLYAT == 3
void
get_it(void)
{
int					sockfd;
	struct ifi_info			*ifi;
	unsigned char		*ptr;
	struct arpreq		arpreq;
	struct sockaddr_in	*sin;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	for (ifi = get_ifi_info(AF_INET, 0); ifi != NULL; ifi = ifi->ifi_next) {
		printf("%s: ", Sock_ntop(ifi->ifi_addr, sizeof(struct sockaddr_in)));

		sin = (struct sockaddr_in *) &arpreq.arp_pa;
		memcpy(sin, ifi->ifi_addr, sizeof(struct sockaddr_in));

		if (ioctl(sockfd, SIOCGARP, &arpreq) < 0) {
			err_ret("ioctl SIOCGARP");
			continue;
		}

		ptr = &arpreq.arp_ha.sa_data[0];
		printf("%x:%x:%x:%x:%x:%x\n", *ptr, *(ptr+1),
			   *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
	}
}

#elif BLYAT == 4
void
get_it(void)
{
	int sd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr[16];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(ifr);
	ifc.ifc_buf = (caddr_t)ifr;
	char host[300];

	ioctl(sd, SIOCGIFCONF, &ifc);
	int ifc_num = ifc.ifc_len / sizeof(struct ifreq);
	printf("%d interfaces found\n", ifc_num);

	for (int i = 1, arp_pa, arp_ha, addr; i < ifc_num; ++i) {
		if (ifr[i].ifr_addr.sa_family != AF_INET) {
			continue;
		}

		printf("%d) interface: %s\n", i + 1, ifr[i].ifr_name);

		ioctl(sd, SIOCGIFADDR, &ifr[i]);
		addr =
		 ((struct sockaddr_in *)(&ifr[i].ifr_addr))->sin_addr.s_addr;
		printf("%d) address: %d.%d.%d.%d\n", i + 1, INT_TO_ADDR(addr));

		ioctl(sd, SIOCGARP, &ifr[i]);
		// addr =
		//     ((struct sockaddr_in
		//     *)(&ifr[i].ifr_addr))->sin_addr.s_addr;
		// printf("%d) address: %d.%d.%d.%d\n", i + 1,
		// INT_TO_ADDR(addr));
	}
	close(sd);
}

#else
void
get_it(void)
{
	struct ifaddrs *ifaddr;

	char host[NI_MAXHOST];

	getifaddrs(&ifaddr);
#	pragma unroll 2
	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_PACKET &&
		 ifa->ifa_data != NULL) {
			struct rtnl_link_stats *stats = ifa->ifa_data;

			printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
			       "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
			 stats->tx_packets, stats->rx_packets, stats->tx_bytes,
			 stats->rx_bytes);
		}

		if (ifa->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		/* Display interface name and family (including symbolic
		   form of the latter for the common families). */

		printf("%s %s %hu %s %hu\n", ifa->ifa_name,
		 (ifa->ifa_addr)->sa_data, (ifa->ifa_addr)->sa_family,
		 (ifa->ifa_netmask)->sa_data, (ifa->ifa_netmask)->sa_family);

		// printf("suka: %s\n",
		//        inet_ntoa(
		// 	   (((struct sockaddr_in *)ifa->ifa_addr)->sin_addr)));

		getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
		 sizeof(host), 0, 0, NI_NUMERICHOST);

		printf("blyat: <%s>\n", host);
	}

	freeifaddrs(ifaddr);
}
#endif

#if 0
void
get_it(void)
{
	struct ifaddrs *addresses;
	getifaddrs(&addresses);

	struct ifaddrs *address = addresses;
	while (address) {
		int family = address->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6) {
			printf("%s\t", address->ifa_name);
			printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
			char ap[100];
			const int family_size =
			    family == AF_INET ? sizeof(struct sockaddr_in)
					      : sizeof(struct sockaddr_in6);
			getnameinfo(address->ifa_addr, family_size, ap,
				    sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
		}
		address = address->ifa_next;
	}
	freeifaddrs(addresses);
}

#endif

#if 0
void get_it(void)
{
	struct ifaddrs *ifaddr;
	struct ifaddrs *ifa;
	int family;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
				NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

		if ((strcmp(ifa->ifa_name, "wlan0") == 0) &&
		    (ifa->ifa_addr->sa_family == AF_INET)) {
			if (s != 0) {
				printf("getnameinfo() failed: %s\n",
				       gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			printf("\tInterface : <%s>\n", ifa->ifa_name);
			printf("\t  Address : <%s>\n", host);
		}
	}
	freeifaddrs(ifaddr);
}
#endif
