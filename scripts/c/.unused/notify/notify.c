#include "../my_macros.h"
#include <dbus-1.0/dbus/dbus.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static const char *summary_global;
static unsigned int id_global;
static int timeout_global;

#define NOTIFICATION_DESTINATION "org.freedesktop.Notifications"
#define NOTIFICATION_PATH "/org/freedesktop/Notifications"
#define NOTIFICATION_IFACE NOTIFICATION_DESTINATION
#define NOTIFICATION_METHOD "Notify"

static void
run_notification()
{
	DBusConnection *connection;
	DBusMessage *message;
	DBusMessageIter iter[4];
	const char *const application = "";
	const char *const icon = "";
	const char *const body = "";
	const char *const urgency = "";
	/*
	 * #define LOW_URGENCY 0
	 * #define NORMAL_URGENCY 1
	 * #define CRITICAL_URGENCY 2
	 */
	const unsigned char level = 0;

	connection = dbus_bus_get(0, 0);
	CHECK(!connection);

	message = dbus_message_new_method_call(NOTIFICATION_DESTINATION,
	 NOTIFICATION_PATH, NOTIFICATION_IFACE, NOTIFICATION_METHOD);
	CHECK(!message);
	dbus_message_iter_init_append(message, iter);
	CHECK(!dbus_message_iter_append_basic(iter, 's', &application));
	CHECK(!dbus_message_iter_append_basic(iter, 'u', &id_global));
	CHECK(!dbus_message_iter_append_basic(iter, 's', &icon));
	CHECK(!dbus_message_iter_append_basic(iter, 's', &summary_global));
	CHECK(!dbus_message_iter_append_basic(iter, 's', &body));
	CHECK(!dbus_message_iter_open_container(iter, 'a', "s", iter + 1));
	CHECK(!dbus_message_iter_close_container(iter, iter + 1));
	CHECK(!dbus_message_iter_open_container(iter, 'a', "{sv}", iter + 1));
	CHECK(!dbus_message_iter_open_container(iter + 1, 'e', 0, iter + 2));
	CHECK(!dbus_message_iter_append_basic(iter + 2, 's', &urgency));
	CHECK(!dbus_message_iter_open_container(iter + 2, 'v', "y", iter + 3));
	CHECK(!dbus_message_iter_append_basic(iter + 3, 'y', &level));
	CHECK(!dbus_message_iter_close_container(iter + 2, iter + 3));
	CHECK(!dbus_message_iter_close_container(iter + 1, iter + 2));
	CHECK(!dbus_message_iter_close_container(iter, iter + 1));
	CHECK(!dbus_message_iter_append_basic(iter, 'i', &timeout_global));
	CHECK(!dbus_connection_send(connection, message, 0));
	{
		sleep(2);
		printf("%d\n",
		 dbus_message_is_signal(
		  message, NOTIFICATION_IFACE, "ActionInvoked"));
	}
	dbus_connection_flush(connection);
	dbus_message_unref(message);
	dbus_connection_unref(connection);
}

#define MAX_FILENAME_LENGTH 256
#define TMP_STR1 "unix:path=/run/user/"
static void
send_notification_to_user(struct dirent *const namelist)
{
	const char *const uid = namelist->d_name;
	const unsigned int uid_num = (unsigned int)atoi(uid);
	char dbus_session_bus_address[MAX_FILENAME_LENGTH] = TMP_STR1;
	const unsigned long d_namelen = strlen(uid);
	CHECK(uid_num == 0);
	setuid(uid_num);

	memcpy(dbus_session_bus_address + STRLEN(TMP_STR1), uid, d_namelen);
	free(namelist);
#define TMP_STR2 "/bus"
	memcpy(dbus_session_bus_address + STRLEN(TMP_STR1) + d_namelen,
	 TMP_STR2, sizeof(TMP_STR2));
	/*
	 * dbus_session_bus_address should end up being something like this:
	 * "unix:path=/run/user/1000/bus"
	 */
	setenv("DBUS_SESSION_BUS_ADDRESS", dbus_session_bus_address, 1);
	run_notification();
}

#define MAX_LOGGED_IN_USERS 256

#if 0
#	define MAX_PATH_LENGTH 32
#	define NOTIFICATION_DAEMON "dunst"
static void
run_notification_daemon()
{
	DIR *dir;
	const struct dirent *dp;
	char comm_path[MAX_PATH_LENGTH] = "/proc/";
	char *comm_path_ptr;
	char *const temp_ptr = comm_path + sizeof("/proc/") - 1;
	unsigned long d_namelen;
	char program_name[sizeof(NOTIFICATION_DAEMON)];
	FILE *fp;
	dir = opendir("/proc");
	CHECK(!dir);
	for (dp = readdir(dir); dp; dp = readdir(dir)) {
		if (dp->d_name[0] > '9' || dp->d_name[0] < '0') {
			continue;
		}
		comm_path_ptr = temp_ptr;
		d_namelen = strlen(dp->d_name);
		memcpy(comm_path_ptr, dp->d_name, d_namelen);
		comm_path_ptr += d_namelen;
		memcpy(comm_path_ptr, "/comm", sizeof("/comm"));
		fp = fopen(comm_path, "r");
		CHECK(!fp);
		CHECK(!fgets(program_name, sizeof(NOTIFICATION_DAEMON), fp));
		if (!memcmp(program_name, NOTIFICATION_DAEMON,
		     sizeof(NOTIFICATION_DAEMON))) {
			CHECK(fclose(fp));
			CHECK(closedir(dir));
			return;
		}
		CHECK(fclose(fp));
	}
	CHECK(closedir(dir));
	if (!fork()) {
		CHECK(close(2));
		execlp(NOTIFICATION_DAEMON, NOTIFICATION_DAEMON, 0);
	}
	/* Wait for the notification daemon to actually start working. */
	sleep(1);
}
#endif

static void
notify_send(const char *const summary, const unsigned int id, const int timeout)
{
	struct dirent **namelist;
	int pid[MAX_LOGGED_IN_USERS];
	int n;
	unsigned char i = 0;

	summary_global = summary;
	id_global = id;
	timeout_global = timeout;

	if (getuid() != 0) {
		pid[i] = fork();
		CHECK(pid[i] == -1);
		if (pid[i]) {
			waitpid(pid[i], 0, 0);
		} else {
			run_notification();
		}
		return;
	}

	for (n = scandir("/run/user", &namelist, 0, 0); n--;
	     free(namelist[n])) {
		switch (namelist[n]->d_name[0]) {
		case '.': /* FALLTHROUGH */
		case '0':
			continue;
		default:
			break;
		}
		pid[i] = fork();
		CHECK(pid[i] == -1);
		if (!pid[i]) {
			send_notification_to_user(namelist[n]);
			_exit(EXIT_SUCCESS);
		}
		++i;
	}
	for (n = 0; n < i; n++) {
		waitpid(pid[n], 0, 0);
	}
	free(namelist);
}

#ifndef NO_MAIN
int
main(const int argc, const char *const *const argv)
{
	if (argc == 4) {
		notify_send(
		 argv[1], (unsigned int)atoi(argv[2]), atoi(argv[3]));
	} else {
		notify_send("Good luck!", 47, 1000);
	}
	dbus_shutdown();
	return EXIT_SUCCESS;
}
#endif
