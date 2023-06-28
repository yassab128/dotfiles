#if 0
true; exec ccompile "$0" -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include \
	-lgio-2.0 -lglib-2.0 -- "$@"
#endif

#include <stdio.h>

#include <time.h>
static int my_sleep(long seconds)
{
	return nanosleep(&(struct timespec){.tv_sec = seconds, .tv_nsec = 0},
		0);
}

#include <gio/gio.h>
#define RUN_AS_CHILD 0
#if RUN_AS_CHILD
#	include <sys/wait.h>
#endif
static void notify_send(char *message, unsigned char notification_id,
	int timeout)
{
#if RUN_AS_CHILD
	int pid = fork();
	if (pid) {
		waitpid(pid, 0, 0);
	} else {
#endif
		GDBusConnection *connection = g_bus_get_sync(G_BUS_TYPE_SESSION,
			0, 0);
		GDBusProxy *proxy = g_dbus_proxy_new_sync(connection, 0, 0,
				"org.freedesktop.Notifications",
				"/org/freedesktop/Notifications",
				"org.freedesktop.Notifications", 0, 0);
		GVariant *key = g_variant_new_string("");
		GVariant *value = g_variant_new_variant(g_variant_new_byte(0));
		GVariant *notify_entry = g_variant_new_dict_entry(key, value);
		GVariant *notify_hints = g_variant_new_array(0, &notify_entry,
			1);

		GVariantBuilder builder = {0};
		g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);
		g_variant_builder_add(&builder, "s", "");
		GVariant *parameters = g_variant_new("(susssas@a{sv}i)", "",
			notification_id, "", message, "", &builder,
			notify_hints, timeout);
		g_dbus_proxy_call_sync(proxy, "Notify",
			parameters, 0, -1, 0, 0);
#if RUN_AS_CHILD
		quick_exit(0);
	}
#endif
}

int main()
{
	notify_send("fjkdsjfkks", 73, 1000);
}
