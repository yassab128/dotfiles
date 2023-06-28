#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

int
main()
{
	Atom clip, utf8, type;
	Display *dpy;
	Window win;
	XEvent ev;
	int fmt;
	long off = 0;
	unsigned char *data;
	unsigned long len, more;

	dpy = XOpenDisplay(0);

	utf8 = XInternAtom(dpy, "UTF8_STRING", False);
	clip = XInternAtom(dpy, 0, False);
	win =
	 XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1, 1, 0, 0, 0);
	XConvertSelection(dpy, XA_PRIMARY, utf8, clip, win, CurrentTime);

	XNextEvent(dpy, &ev);
	if (ev.type == SelectionNotify && ev.xselection.property != None) {
		do {
			XGetWindowProperty(dpy, win, ev.xselection.property,
			 off, BUFSIZ / 4, False, utf8, &type, &fmt, &len, &more,
			 &data);
			fwrite(data, 1, len, stdout);
			XFree(data);
		} while (more > 0);
		putchar('\n');
	}
	XCloseDisplay(dpy);
	return 0;
}
