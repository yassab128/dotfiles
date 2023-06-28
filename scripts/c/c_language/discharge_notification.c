//true; exec ccompile "$0" -D_XOPEN_SOURCE -I/usr/include/gdk-pixbuf-2.0 -I\
//usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libpng16 -I\
//usr/include/libmount -I/usr/include/blkid\
//-pthread -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 "$@"

#include <libnotify/notify.h>

/* #define -D_XOPEN_SOURCE 1 */
/* For putenv */
#include <stdlib.h>

static void notify_send(char *message, unsigned short timeout)
{
	notify_init("m");
	NotifyNotification *notification = notify_notification_new(message, 0, 0);
	notify_notification_set_timeout(notification, timeout);
	notify_notification_show(notification, 0);
	g_object_unref(G_OBJECT(notification));
	notify_uninit();
}

int main() {
	FILE *file;
	file = fopen("/sys/class/power_supply/AC/online", "re");
	int battery_status = fgetc(file);
	fclose(file);

	/* It seems like single quotes must be used with chars. */
	if (battery_status == '1') {
		putenv("DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus");
		notify_send("The battery is draining", 10000);
	}
}
