#pragma GCC diagnostic push
/* #pragma GCC diagnostic ignored "-Wreserved-id-macro" */
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#include <X11/Xlib.h>
#pragma GCC diagnostic pop
#include <stdio.h>

#include <sys/select.h>

#include <sys/time.h>

#include "../my_macros.h"

/* -100 <= BELL_VOLUME <= 100 */
#define BELL_VOLUME 100

/*
 * The color codes are in the hexadecimal form.
 * See the next url for examples:
 * https://en.wikipedia.org/wiki/List_of_colors:_A-F
 */
#define BLACK 0x0
#define RED 0xFF0000
#define YELLOW 0xFFCC00

/*
 * Xlib only supports XLFD fonts, run `xlsfonts` to list theavailable
 * ones on your system. DejaVu should be available on most distributions,
 * but if you don't have it, set USE_DEFAULT_FONT to 1, otherwise the
 * program will crash.
 */
#define USE_DEFAULT_FONT 0
#if USE_DEFAULT_FONT
#	define FONT "fixed"
#else
#	define FONT                                                           \
		"-misc-dejavu serif-medium-r-normal--0-0-0-0-p-0-iso10646-1"
#endif

#define WINDOW_X 0
#define WINDOW_Y 50
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 40
#define WINDOW_BORDER_WIDTH 5

#define TEXT_X 0
#define TEXT_Y 20

static void
update_timeout(const struct timeval time_a, const struct timeval time_b,
 struct timeval *const time_remaining)
{
	time_remaining->tv_usec += time_a.tv_usec - time_b.tv_usec;
	if (time_remaining->tv_usec < 0) {
		time_remaining->tv_usec += 1000000;
		time_remaining->tv_sec += time_a.tv_sec - time_b.tv_sec - 1;
	} else if (time_remaining->tv_usec > 1000000) {
		time_remaining->tv_usec -= 1000000;
		time_remaining->tv_sec += time_a.tv_sec - time_b.tv_sec + 1;
	} else {
		time_remaining->tv_sec += time_a.tv_sec - time_b.tv_sec;
	}
}

static unsigned char
xnotify(const char *const display_name, const long timeout_seconds,
 const long timeout_microseconds, const char *const message,
 const int message_length)
{
	Window w;
	GC gc;
	Display *display;
	XEvent event_return;
	struct timeval time_remaining;
	struct timeval time_a;
	struct timeval time_b;
	int fd;
	fd_set set;

	display = XOpenDisplay(display_name);
	if (!display) {
		fputs("XOpenDisplay(): cannot open display\n", stderr);
		return 1;
	}
	/*
	 * Not much error handling is needed from now on, because non-successful
	 * Xlib functions will crash the program anyway and will generate an
	 * error message on their own.
	 */

	{
		XSetWindowAttributes attributes;
		attributes.background_pixel = BLACK;
		attributes.border_pixel = RED;
		attributes.override_redirect = True;
		w = XCreateWindow(display, XRootWindow(display, CopyFromParent),
		 WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT,
		 WINDOW_BORDER_WIDTH, CopyFromParent, InputOutput,
		 CopyFromParent,
		 CWBackPixel | CWBorderPixel | CWOverrideRedirect, &attributes);
	}

	XSelectInput(display, w, ExposureMask | ButtonPressMask);

	XMapWindow(display, w);

	{
		XGCValues values;
		values.foreground = YELLOW;
		values.font = XLoadFont(display, FONT);
		gc = XCreateGC(display, w, GCForeground | GCFont, &values);
	}

	/* Prepare the arguments of select(). */
	fd = ConnectionNumber(display);
	FD_ZERO(&set);
	FD_SET(fd, &set);
	time_remaining.tv_sec = timeout_seconds;
	time_remaining.tv_usec = timeout_microseconds;

	/*
	 * ExposureMask's event always (?) gets triggered first, so there is
	 * is no point in iterating through it. Hence, XNextEvent is called.
	 * before the loop.
	 */
	XNextEvent(display, &event_return);
	XBell(display, BELL_VOLUME);
	XDrawString(display, w, gc, TEXT_X, TEXT_Y, message, message_length);
	XFlush(display);
	if (gettimeofday(&time_a, 0) == -1) {
		perror("gettimeofday()");
		XFreeGC(display, gc);
		XCloseDisplay(display);
		return 1;
	}
	for (;;) {
		switch (select(fd + 1, &set, 0, 0, &time_remaining)) {
		case -1:
			/* Error */
			XFreeGC(display, gc);
			XCloseDisplay(display);
			return 1;
		case 0:
			/* Event timed out */
			break;
		default:
			/* Event triggered */
			XNextEvent(display, &event_return);
			if (event_return.type == ButtonPress) {
				break;
			}
			if (gettimeofday(&time_b, 0) == -1) {
				perror("gettimeofday()");
				XFreeGC(display, gc);
				XCloseDisplay(display);
				return 1;
			}
			update_timeout(time_a, time_b, &time_remaining);
			XBell(display, BELL_VOLUME);
			XDrawString(display, w, gc, TEXT_X, TEXT_Y, message,
			 message_length);
			XFlush(display);
			/*
			 * Call gettimeofday() last so that the timeout
			 * won't be affected much by function calls overhead.
			 */
			if (gettimeofday(&time_a, 0) == -1) {
				perror("gettimeofday()");
				XFreeGC(display, gc);
				XCloseDisplay(display);
				return 1;
			}
			continue;
		}
		break;
	}
	XFreeGC(display, gc);
	XCloseDisplay(display);
	return 0;
}

#define MSG "Hello, world!"
#define TIMEOUT_SECONDS 5
#define TIMEOUT_MICROSECONDS 30000
int
main()
{
	xnotify(
	 ":0", TIMEOUT_SECONDS, TIMEOUT_MICROSECONDS, MSG, sizeof(MSG) - 1);
	return 0;
}
