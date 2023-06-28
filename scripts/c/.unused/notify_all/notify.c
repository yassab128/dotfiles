#include "../my_macros.h"
#include <glob.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define XAUTHORITY_PATH_MAX 64
#define DISPLAY_SIZE sizeof("DISPLAY=:0")

/* Does not support sending notification to more than 10 logged-in users. */
#define MAX_LOGGED_IN_USERS 11

static void
send_message(char *const *const xmessage_argv)
{
	int pid[MAX_LOGGED_IN_USERS];
	unsigned char i = 0;
	unsigned char j = 0;
	char *envp[3];

	char display[DISPLAY_SIZE];
	char xauthority[XAUTHORITY_PATH_MAX];
	unsigned char display_number;
	glob_t globbuf;
	FILE *fp;

	envp[2] = (char *)0;
	/***********************************************************************/
	CHECK(glob("/home/*/.Xauthority", GLOB_NOSORT, 0, &globbuf));
	for (; i != globbuf.gl_pathc; ++i) {
		fp = fopen(globbuf.gl_pathv[i], "r");
		if (!fp) {
			continue;
		}
		CHECK(fscanf(fp, "%*[^0-9]%hhu", &display_number) != 1);
		fclose(fp);
		CHECK(snprintf(display, DISPLAY_SIZE, "DISPLAY=:%hhu",
		       display_number) == -1);
		CHECK(snprintf(xauthority, XAUTHORITY_PATH_MAX, "XAUTHORITY=%s",
		       globbuf.gl_pathv[i]) == -1);
		envp[0] = display;
		envp[1] = xauthority;

		pid[j] = fork();
		switch (pid[j]) {
		case 0:
			globfree(&globbuf);
			execvpe(xmessage_argv[0], xmessage_argv, envp);
			_exit(1);
		case -1:
			perror("fork");
			_exit(1);
		default:
			break;
		}
		++j;
	}
	globfree(&globbuf);
	/***********************************************************************/
	for (i = 0; i < j; ++i) {
		putc('\a', stderr);
		CHECK(waitpid(pid[i], 0, 0) == -1);
	}
}

#define MAX_XMESSAGE_ARGV 64

static void
notify_all(char *count, ...)
{
	va_list ap;
	unsigned char i = 0;
	char *xmessage_argv[MAX_XMESSAGE_ARGV];
	va_start(ap, count);

	xmessage_argv[i] = count;
	while (count) {
		++i;
		count = va_arg(ap, char *);
		xmessage_argv[i] = count;
	}
	va_end(ap);
	send_message(xmessage_argv);
}

#ifndef NO_MAIN
int
main(const int argc, const char *const *const argv)
{
	if (argc != 3) {
		return 1;
	}
	notify_all(
	 "xmessage", "-buttons", "", "-timeout", argv[2], argv[1], (char *)0);
	return 0;
}
#endif
