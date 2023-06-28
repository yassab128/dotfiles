/* Doesn't work on Android without root privileges. */
#include <assert.h>
#include <linux/wireless.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int
main()
{
	ssize_t retval;
	struct ifreq ifr[IW_MAX_AP];
	int sockfd;
	int i;
	struct iwreq wrq;
	struct iw_statistics iwstats;
	char essid_name[IW_ESSID_MAX_SIZE];
	const short wireless_flags =
	    IFF_MULTICAST | IFF_BROADCAST | IFF_RUNNING;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd != -1) {
		struct ifconf ifc;

		ifc.ifc_buf = (char *)ifr;
		ifc.ifc_len = sizeof(ifr);
		retval = ioctl(sockfd, SIOCGIFCONF, &ifc);
		assert(retval == 0);
		i = ifc.ifc_len / (int)sizeof(struct ifreq);
	} else {
		perror("socket");
		return 1;
	}

	while (i--) {
		if (ioctl(sockfd, SIOCGIFFLAGS, &ifr[i]) != 0) {
			continue;
		}
		if ((ifr[i].ifr_flags & wireless_flags) == wireless_flags) {
			break;
		}
	}

	strcpy(wrq.ifr_name, ifr[i].ifr_name);

	wrq.u.essid.pointer = essid_name;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE;

	retval = ioctl(sockfd, SIOCGIWESSID, &wrq);
	if (retval == 0) {
		/* Add the NULL terminator. */
		essid_name[wrq.u.essid.length] = 0;
		printf("essid: %s\n", essid_name);
	}

	wrq.u.data.pointer = &iwstats;
	wrq.u.data.length = sizeof(iwstats);

	retval = ioctl(sockfd, SIOCGIWSTATS, &wrq);
	if (retval == 0) {
		printf("signal strength: %hhu\n",
		    (unsigned char)((iwstats.qual.level + 100) << 1));
	}

	retval = close(sockfd);
	assert(retval == 0);

	return 0;
}
