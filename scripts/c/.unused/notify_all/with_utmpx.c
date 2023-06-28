#include "../my_macros.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmpx.h>

#define MAX_XAUTHORITY_VALUE 64
#define DISPLAY_SIZE sizeof("DISPLAY=:0")

/* Does not support sending notification to more than 10 logged-in users. */
#define MAX_LOGGED_IN_USERS 11

static void
notify_all(char *message)
{
	char *xmessage_argv[3];
	int pid[MAX_LOGGED_IN_USERS];
	unsigned char i = 0;
	char *envp[3];

	struct utmpx *entry;
	char display[DISPLAY_SIZE] = "DISPLAY=:";
	char xauthority[MAX_XAUTHORITY_VALUE] = "XAUTHORITY=/home/";
	char *xauthority_offset = xauthority + STRLEN("XAUTHORITY=/home/");
	char *xauthority_ptr;
	unsigned long username_lenght;
	unsigned int user_uid = getuid();

	xmessage_argv[0] = "xmessage";
	xmessage_argv[1] = message;
	xmessage_argv[2] = (char *)0;

	envp[2] = (char *)0;
	for (entry = getutxent(); entry; entry = getutxent()) {
		printf("%d %s %s %s %s %d\n", entry->ut_pid, entry->ut_line,
		 entry->ut_id, entry->ut_user, entry->ut_host,
		 entry->ut_session);
		if (entry->ut_host[0] != ':' || entry->ut_host[2] != 0) {
			continue;
		}
		username_lenght = strlen(entry->ut_user);
		if (user_uid != 0 &&
		 memcmp(getlogin(), entry->ut_user, username_lenght) != 0) {
			continue;
		}
		display[DISPLAY_SIZE - 2] = entry->ut_host[1];
		xauthority_ptr = xauthority_offset;
		memcpy(xauthority_ptr, entry->ut_user, username_lenght);
		xauthority_ptr += username_lenght;
		memcpy(xauthority_ptr, "/.Xauthority", STRLEN("/.Xauthority"));

		envp[0] = display;
		envp[1] = xauthority;

		pid[i] = fork();
		switch (pid[i]) {
		case 0:
			endutxent();
			printf("%s %s\n", envp[0], envp[1]);
			execvpe(xmessage_argv[0], xmessage_argv, envp);
			_exit(1);
		case -1:
			perror(0);
			_exit(1);
		default:
			break;
		}
		++i;
	}
	endutxent();
	while (i--) {
		waitpid(i, 0, 0);
	}
}

int
main()
{
	notify_all("Guten Tag\n");
	return EXIT_SUCCESS;
}
