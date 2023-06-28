#include "../my_macros.h"
#include "get_router_gateway.h"
#include <errno.h>
#include <linux/rtnetlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#if 0
/* An obsolete way to do it. */
static void
get_gateway_alternative(char *gateway)
{
	int fd1[2];
	CHECK(pipe(fd1));
	const int pid = fork();
	if (pid == -1) {
		perror("fork()");
		quick_exit(1);
	} else if (!pid) {
		CHECK(close(fd1[0]));
		CHECK(dup2(fd1[1], 1) == -1);
		CHECK(close(fd1[1]));
		CHECK(execlp("ip", "ip", "-4", "n", (char *)0));
	}
	CHECK(close(fd1[1]));

	FILE *const fp = fdopen(fd1[0], "r");
	CHECK(fscanf(fp, "%s", gateway) != 1);
	CHECK(fclose(fp));

	/*
	printf("fd: %d\n", fcntl(fd1[0], F_GETFD));
	CHECK(close(fd1[0]));
	*/
	CHECK(waitpid(pid, 0, 0) == -1);
}

static void
get_gateway(char *gateway)
{
	FILE *fp = fopen("/proc/net/arp", "r");
	if (!fp) {
		get_gateway_alternative(gateway);
	} else {
		CHECK(fscanf(fp, "%*[^\n]\n%s", gateway) != 1);
		CHECK(fclose(fp));
	}
}
#endif

static int
rtnl_print_neigh(struct nlmsghdr *h)
{
	const struct ndmsg *const addr = NLMSG_DATA(h);
	struct rtattr *attr;
	long len = NLMSG_PAYLOAD(h, sizeof(struct ndmsg));
	/*
	if (addr->ndm_type != NDA_DST) {
		return EXIT_FAILURE;
	}
	*/
	if (addr->ndm_state == NUD_NOARP) {
		return -1;
	}

	for (attr = NDA_RTA(addr);; attr = MY_RTA_NEXT(attr, len)) {
		if (attr->rta_type == NDA_DST) {
			return *(int *)RTA_DATA(attr);
		}
	}

	/*
	printf("%d %d %d %d %d\n", addr->ndm_family, addr->ndm_ifindex,
	 addr->ndm_state, addr->ndm_flags, addr->ndm_type);
	*/
}

#define BUFSIZE 8192

int
get_router_ipv4_gateway()
{
	struct {
		struct nlmsghdr hdr;
		struct rtgenmsg gen;
		const char pad[3];
	} req = {0};
	int ipv4;
	int somelom;
	struct iovec iov;
	struct msghdr msg = {0};
	struct nlmsghdr buff[BUFSIZE];
	long len;
	struct nlmsghdr *msg_ptr;

	req.hdr.nlmsg_len = sizeof(req);
	req.hdr.nlmsg_type = RTM_GETNEIGH;
	req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
	req.gen.rtgen_family = AF_INET;

	iov.iov_base = &req;
	iov.iov_len = req.hdr.nlmsg_len;

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	somelom = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	CHECK(somelom == -1);

	CHECK(sendmsg(somelom, &msg, 0) == -1);

	msg.msg_iov->iov_base = buff;
	msg.msg_iov->iov_len = BUFSIZE;
	len = recvmsg(somelom, &msg, 0);
	CHECK(len == -1);
	for (msg_ptr = (struct nlmsghdr *)buff;
	     NLMSG_OK(msg_ptr, (unsigned int)len);
	     msg_ptr = MY_NLMSG_NEXT(msg_ptr, len)) {
		ipv4 = rtnl_print_neigh(msg_ptr);
		if (ipv4 != -1) {
			CHECK(close(somelom) == -1);
			return ipv4;
		}
	}
	CHECK(close(somelom) == -1);
	errno = ENETUNREACH;
	return -1;
}

#ifndef NO_MAIN
int
main()
{
	const int gateway = get_router_ipv4_gateway();
	CHECK(gateway == -1);
	printf("%d.%d.%d.%d\n", INT_TO_IPV4_CONVERTER(gateway));
	return EXIT_SUCCESS;
}
#endif
