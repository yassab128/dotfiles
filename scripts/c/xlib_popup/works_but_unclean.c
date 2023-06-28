#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG "Hello, world!"

#define USE_CUSTOM_FONT 0

int
main()
{
	Display *dpy;
	Window parent_window;
	XEvent e;
	unsigned long black_color;
	unsigned long white_color;
	int default_screen;
#if USE_CUSTOM_FONT
	Font font;
#endif
	Visual *visual;
	int depth;
	XSetWindowAttributes attributes;

	dpy = XOpenDisplay(0);
#if USE_CUSTOM_FONT
	font = XLoadFont(
	 dpy, "-misc-dejavu sans mono-medium-r-normal--0-0-0-0-m-0-iso10646-1");
	if (!font) {
		font = XLoadFont(dpy, "fixed");
	}
#endif

	default_screen = DefaultScreen(dpy);
	visual = DefaultVisual(dpy, default_screen);
	depth = DefaultDepth(dpy, default_screen);

	attributes.background_pixel = XWhitePixel(dpy, default_screen);
	attributes.border_pixel = XBlackPixel(dpy, default_screen);
	attributes.override_redirect = True;
	attributes.win_gravity = NorthWestGravity;
#if 1
	/* parent_window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 10,
	 10, 20, 20, 1, black_color, white_color); */

	/* parent_window = XCreateWindow(dpy, DefaultRootWindow(dpy), 9, 9, 15,
	 20, 5, CopyFromParent, CopyFromParent, CopyFromParent, 0, 0); */

	parent_window = XCreateWindow(dpy, XRootWindow(dpy, default_screen), 0,
	 20, 400, 40, 0, depth, InputOutput, visual,
	 CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWWinGravity,
	 &attributes);
#else
	parent_window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 10, 10,
	 20, 20, 1, black_color, white_color);
#endif

#if 0
	XSelectInput(dpy, parent_window, ExposureMask | KeyPressMask);
#else
	XSelectInput(dpy, parent_window, ExposureMask | ButtonPressMask);
#endif

	/* XStoreName(dpy, parent_window, "floating"); */
	XMapWindow(dpy, parent_window);
	/* XFlush(dpy); */

	XSetInputFocus(dpy, parent_window, RevertToParent, CurrentTime);
	while (1) {
		puts("1");
		XNextEvent(dpy, &e);
		puts("2");
		if (e.type == Expose) {
			XDrawString(dpy, parent_window,
			 DefaultGC(dpy, default_screen), 0, 10, MSG,
			 sizeof(MSG) - 1);
		}
		printf("%d\n", e.type);
		if (e.type == ButtonPress) {
			break;
		}
	}

#if USE_CUSTOM_FONT
	/* XFreeFont(dpy, font); */
#endif
	XCloseDisplay(dpy);
	return 0;
}
