#include "../my_macros.h"
#include <glob.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmpx.h>

#define XAUTHORITY_PATH_MAX 64
#define DISPLAY_SIZE sizeof("DISPLAY=:0")

/* Does not support sending notification to more than 10 logged-in users. */
#define MAX_LOGGED_IN_USERS 11

static void
notify_all(char *message)
{
	char *xmessage_argv[3];
	int pid[MAX_LOGGED_IN_USERS];
	unsigned char i = 0;
	unsigned char j = 0;
	char *envp[3];

	char display[DISPLAY_SIZE];
	char xauthority[XAUTHORITY_PATH_MAX];
	unsigned char display_number;
	glob_t globbuf;
	FILE *fp;

	xmessage_argv[0] = "xmessage";
	xmessage_argv[1] = message;
	xmessage_argv[2] = (char *)0;

	envp[2] = (char *)0;
	/***********************************************************************/
	CHECK(glob("/home/*/.Xauthority", GLOB_NOSORT, 0, &globbuf));
	for (; i != globbuf.gl_pathc; ++i) {
		fp = fopen(globbuf.gl_pathv[i], "r");
		if (!fp) {
			continue;
		}
		fscanf(fp, "%*[^0-9]%hhu", &display_number);
		fclose(fp);
		snprintf(
		 display, DISPLAY_SIZE, "DISPLAY=:%hhu", display_number);
		snprintf(xauthority, XAUTHORITY_PATH_MAX, "XAUTHORITY=%s",
		 globbuf.gl_pathv[i]);
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
		waitpid(pid[i], 0, 0);
	}
}

int
main()
{
	notify_all("Guten Tag\n");
	return EXIT_SUCCESS;
}
