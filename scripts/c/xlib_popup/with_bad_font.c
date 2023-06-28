#include <X11/Xlib.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../my_macros.h"

#define DIE(str)                                                               \
	do {                                                                   \
		fputs(str, stderr);                                            \
		_exit(EXIT_FAILURE);                                           \
	} while (0)

/* -100 <= BELL_VOLUME <= 100 */
#define BELL_VOLUME 100

#define BLACK 0x0
#define RED 0xFF0000
#define YELLOW 0xFFCC00

static void
xmessage(const char *const message, const int message_length, const int timeout)
{
	Window w;
	int screen_number;
	int depth;
	struct pollfd pfds[1];
	Visual *visual;
	XSetWindowAttributes attributes;
	GC gc;

	Display *const display = XOpenDisplay(0);
	if (!display) {
		DIE("XOpenDisplay(): failed\n");
	}
	screen_number = DefaultScreen(display);
	visual = DefaultVisual(display, screen_number);
	depth = DefaultDepth(display, screen_number);

	attributes.background_pixel = BLACK;
	attributes.border_pixel = RED;

	attributes.override_redirect = True;

	w = XCreateWindow(display, XRootWindow(display, screen_number), 0, 20,
	 400, 40, 5, depth, InputOutput, visual,
	 CWBackPixel | CWBorderPixel | CWOverrideRedirect, &attributes);
	switch (w) {
	case BadAlloc:
		DIE("XCreateWindow(): BadAlloc\n");
	case BadColor:
		DIE("XCreateWindow(): BadColor\n");
	case BadCursor:
		DIE("XCreateWindow(): BadCursor\n");
	case BadMatch:
		DIE("XCreateWindow(): BadMatch\n");
	case BadPixmap:
		DIE("XCreateWindow(): BadPixmap\n");
	case BadValue:
		DIE("XCreateWindow(): BadValue\n");
	case BadWindow:
		DIE("XCreateWindow(): BadWindow\n");
	default:
		break;
	}

	if (XSelectInput(display, w, ButtonPressMask) == BadWindow) {
		DIE("XSelectInput(): BadWindow\n");
	}

	if (XMapWindow(display, w) == BadWindow) {
		DIE("XMapWindow(): BadWindow\n");
	}

	gc = DefaultGC(display, screen_number);
	XSetForeground(display, gc, YELLOW);
	switch (XDrawString(display, w, gc, 0, 10, message, message_length)) {
	case BadDrawable:
		DIE("XDrawString(): BadDrawable\n");
	case BadGC:
		DIE("BadGC(): BadDrawable\n");
	case BadMatch:
		DIE("BadGC(): BadMatch\n");
	default:
		break;
	}

	if (XBell(display, BELL_VOLUME) == BadValue) {
		DIE("XBell(): BadValue\n");
	}

	XFlush(display);

	pfds->fd = ConnectionNumber(display);
	pfds->events = POLLIN;

	if (poll(pfds, 1, timeout) == -1) {
		perror("poll()");
		_exit(EXIT_FAILURE);
	}

	if (XCloseDisplay(display) == BadGC) {
		DIE("XBell(): BadGC\n");
	}
}

#define MSG "Hello, world!"
int
main()
{
	xmessage(MSG, sizeof(MSG) - 1, 500);
	return 0;
}
