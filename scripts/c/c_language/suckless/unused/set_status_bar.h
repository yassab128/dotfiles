//true; exec ccompile "$0" -lX11 -- "$@"

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATE 7
static void set_status(char *msg)
{
	Display *display = XOpenDisplay(0);
	if (!display) {
		return;
	}
	Window rootWindow = DefaultRootWindow(display);

	char buff[DATE];
	strftime(buff, sizeof buff, "%H%M%S",
	localtime_r((time_t[]){time(0)}, &(struct tm){0}));
	char *message = 0;
	asprintf(&message, "%s -> %s", buff, msg);
	XStoreName(display, rootWindow, message);
	free(message);
	XSync(display, False);
	XCloseDisplay(display);
}
