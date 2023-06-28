#include "../my_macros.h"
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#ifndef sa_handler
#	undef sa_handler
#endif

static volatile unsigned char keep_running = 1;

#define BUFF_SIZE sizeof(struct inotify_event) + PATH_MAX + 1

static void
clean_exit(int signum)
{
	(void)signum;
	keep_running = 0;
}

#if 1
#	define FILE_PATH "/sys/class/power_supply/BAT0/capacity"
#else
#	define FILE_PATH "/sys/devices/pci0000:00/0000:00:1c.1/0000:03:00.0/net/wlan0/statistics/rx_bytes"
#endif

int
main()
{
	struct sigaction sa = {{clean_exit}, {{0}}, 0, 0};
	char buff[BUFF_SIZE] = {0};
	const unsigned long buff_size = BUFF_SIZE;
	int fd;
	int wd;
	CHECK(sigaction(SIGINT, &sa, 0) == -1);
	CHECK(sigaction(SIGTERM, &sa, 0) == -1);

	/* struct inotify_event *event = NULL; */

	fd = inotify_init();
	CHECK(fd == -1);
	/* IN_ACCESS */
	wd = inotify_add_watch(fd, FILE_PATH, IN_ALL_EVENTS);
	CHECK(wd == -1);
	CHECK(read(fd, buff, buff_size) == -1);
	puts(buff);
	CHECK(inotify_rm_watch(fd, wd));
	CHECK(close(fd) == -1);
	return EXIT_SUCCESS;
}
