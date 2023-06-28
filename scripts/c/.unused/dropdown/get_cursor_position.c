#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../my_macros.h"

int
main()
{
	Display *d;
	Window inwin;
	Window inchildwin;
	int rootx, rooty;
	int childx, childy;
	Atom atom_type_prop;
	int actual_format;
	unsigned int mask;
	unsigned long n_items, bytes_after_ret;
	Window *props;

	d = XOpenDisplay(0);
	CHECK(!d);

	CHECK(XGetWindowProperty(d, DefaultRootWindow(d),
	 XInternAtom(d, "_NET_ACTIVE_WINDOW", True), 0, 1, False,
	 AnyPropertyType, &atom_type_prop, &actual_format, &n_items,
	 &bytes_after_ret, (unsigned char **)&props));

	CHECK(XQueryPointer(d, *props, &inwin, &inchildwin, &rootx, &rooty,
	       &childx, &childy, &mask) != 1);
	printf("relative to active window: %d,%d\n", childx, childy);
	CHECK(XFree(props) != 1);
	CHECK(XCloseDisplay(d));
	return 0;
}
