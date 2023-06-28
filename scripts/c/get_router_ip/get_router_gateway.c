#include "get_router_gateway.h"
#include <assert.h>
#include <linux/rtnetlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static unsigned int
search_for_wlan_gateway(struct nlmsghdr *const h)
{
	struct rtattr *attr_ptr;
	long attr_len = NLMSG_PAYLOAD(h, sizeof(struct ndmsg));
	struct rtmsg *const rt_msg = NLMSG_DATA(h);

	switch (rt_msg->rtm_type) {
	case RTN_UNREACHABLE:
		return 0;
	default:
		break;
	}

#pragma GCC diagnostic ignored "-Wcast-align"
	for (attr_ptr = RTM_RTA(rt_msg); RTA_OK(attr_ptr, attr_len);
	     attr_ptr = RTA_NEXT(attr_ptr, attr_len)) {
#pragma GCC diagnostic warning "-Wcast-align"
		switch (attr_ptr->rta_type) {
		case RTA_GATEWAY:
#if 0
			printf("%d\n", *(int *)RTA_DATA(attr_ptr));
#endif
			return *(unsigned int *)RTA_DATA(attr_ptr);
		default:
			break;
		}
	}
	return 0;
}

static unsigned int
process_reply(struct nlmsghdr *const buff, unsigned long len)
{
	struct nlmsghdr *msg_ptr;
	unsigned int gateway;
#pragma GCC diagnostic ignored "-Wcast-align"
	for (msg_ptr = buff; NLMSG_OK(msg_ptr, len);
	     msg_ptr = NLMSG_NEXT(msg_ptr, len)) {
#pragma GCC diagnostic warning "-Wcast-align"
		switch (msg_ptr->nlmsg_type) {
		case NLMSG_DONE:
			return 0;
		default:
			gateway = search_for_wlan_gateway(msg_ptr);
			if (gateway) {
				return gateway;
			}
			break;
		}
	}
	return 0;
}

#define BUFSIZE 8192
unsigned int
get_gateway(void)
{
	struct {
		struct nlmsghdr hdr;
		struct rtgenmsg gen;
		const char pad[3];
	} req = {0};
	int somelom;
	struct iovec iov;
	struct msghdr msg = {0};
	struct nlmsghdr buff[BUFSIZE];
	unsigned long len;
	ssize_t ret;
	unsigned int gateway = 0;

	req.hdr.nlmsg_len = sizeof(req);
	req.hdr.nlmsg_type = RTM_GETROUTE;
	req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
	req.gen.rtgen_family = AF_INET;

	iov.iov_base = &req;
	iov.iov_len = req.hdr.nlmsg_len;

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	somelom = socket(
	 AF_NETLINK, SOCK_DGRAM /* SOCK_RAW works too. */, NETLINK_ROUTE);
	assert(somelom != -1);

	ret = sendmsg(somelom, &msg, 0);
	assert(ret != -1);

	msg.msg_iov->iov_base = buff;
	msg.msg_iov->iov_len = BUFSIZE;

	for (;;) {
		len = (unsigned long)recvmsg(somelom, &msg, 0);
		assert(len != -1UL);
		gateway = process_reply(buff, len);
		if (gateway) {
			break;
		}
	}
	ret = close(somelom);
	assert(ret != -1);
	return gateway;
}

#ifndef NO_MAIN
int
main()
{
	unsigned int gateway;
	unsigned char *bytes;
	gateway = get_gateway();
	bytes = (unsigned char *)&gateway;
	printf("%hhu.%hhu.%hhu.%hhu\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}
#endif
