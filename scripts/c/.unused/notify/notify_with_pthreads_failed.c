#include <dbus-1.0/dbus/dbus.h>
#include <stdio.h>
#include <sys/fsuid.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#define STRLEN(x) (sizeof(x) - 1)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CHECK(x)                                                               \
	do {                                                                   \
		if (x) {                                                       \
			perror(TOSTRING(__LINE__) ":" #x);                     \
			/* quick_exit() doesn't exist on some systems. */      \
			_exit(EXIT_FAILURE);                                   \
		}                                                              \
	} while (0)

#define RUN_AS_CHILD 0
#if RUN_AS_CHILD
#	include <sys/wait.h>
#endif

static char *summary;
static unsigned int id;
static int timeout;

static void notify_send()
{
#if RUN_AS_CHILD
	int pid = fork();
	if (pid) {
		waitpid(pid, 0, 0);
		return;
	}
#endif

#if 0
	putenv("DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1001/bus");
#endif
	DBusConnection *connection = dbus_bus_get(0 , 0);
	CHECK(!connection);
	DBusMessage *message = dbus_message_new_method_call(
		"org.freedesktop.Notifications",
		"/org/freedesktop/Notifications",
		"org.freedesktop.Notifications",
		"Notify");
	CHECK(!message);
	DBusMessageIter iter[4];
	dbus_message_iter_init_append(message, iter);
	char *application = "";
	dbus_message_iter_append_basic(iter, 's', &application);
	dbus_message_iter_append_basic(iter, 'u', &id);
	char *icon = "";
	dbus_message_iter_append_basic(iter, 's', &icon);
	dbus_message_iter_append_basic(iter, 's', &summary);
	char *body = "";
	dbus_message_iter_append_basic(iter, 's', &body);
	dbus_message_iter_open_container(iter, 'a', "s", iter + 1);
	dbus_message_iter_close_container(iter, iter + 1);
	dbus_message_iter_open_container(iter, 'a', "{sv}", iter + 1);
	dbus_message_iter_open_container(iter + 1, 'e', 0, iter + 2);
	char *urgency = "urgency";
	dbus_message_iter_append_basic(iter + 2, 's', &urgency);
	dbus_message_iter_open_container(iter + 2, 'v', "y", iter + 3);
	enum {LOW, NORMAL, CRITICAL};
	unsigned char level = LOW;
	dbus_message_iter_append_basic(iter + 3, 'y', &level);
	dbus_message_iter_close_container(iter + 2, iter + 3);
	dbus_message_iter_close_container(iter + 1, iter + 2);
	dbus_message_iter_close_container(iter, iter + 1);
	dbus_message_iter_append_basic(iter, 'i', &timeout);
	dbus_connection_send(connection, message, 0);
	dbus_connection_flush(connection);
	dbus_message_unref(message);
	dbus_connection_unref(connection);
	/* dbus_shutdown(); */
#if RUN_AS_CHILD
	_exit(0);
#endif
}

#define MAX_FILENAME_LENGTH 256
#define MAX_LOGGED_IN_USERS 256
#define TMP_STR "unix:path=/run/user/"
static void *bullshit(void *arg)
{
	const char *uid = (char *)((struct dirent *)arg)->d_name;
	/* setfsuid((unsigned int)atoi(uid)); */
	if (getuid() != 0) {
		puts("works");
		printf("%d\n", setfsuid(0));
	}
	printf("%d\n", setfsuid((unsigned int)atoi(uid)));

	char dbus_session_bus_address[MAX_FILENAME_LENGTH] =
		TMP_STR;
	unsigned long d_namlen = strlen(uid);
	memcpy(dbus_session_bus_address + STRLEN(TMP_STR),
		uid, d_namlen);
	memcpy(dbus_session_bus_address + STRLEN(TMP_STR) + d_namlen,
		"/bus", 5);
	setenv("DBUS_SESSION_BUS_ADDRESS", dbus_session_bus_address, 1);
	free(arg);
	notify_send();
	puts(dbus_session_bus_address);
	return 0;
}


int main()
{
	summary = "Guten Tag!";
	id = 47;
	timeout = 10000;
	if (getuid() == 0) {
		notify_send();
		return EXIT_SUCCESS;
	}

	pthread_t thread_id[MAX_LOGGED_IN_USERS];
	struct dirent **namelist;
	unsigned char i = 0;
	/* for (int n = scandir("/run/user", &namelist, 0, 0); n--;) { */
	for (int k = 0, n = scandir("/run/user", &namelist, 0, 0); k < n; ++k) {
		switch (namelist[k]->d_name[0])
		{
			case '.': /* FALLTHROUGH */
			case '0':
				free(namelist[k]);
				continue;
			default:
				break;
		}
		CHECK(pthread_create(thread_id + i, 0, bullshit,
		 (void *)namelist[k]));
		 ++i;

	}
	while (i--) {
		CHECK(pthread_join(thread_id[i], 0));
	}
	free(namelist);
}
