#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../my_macros.h"

int
main()
{
	Display *dpy;
	Atom *atom;
	char *property;
	int i = 0;
	int natoms;
	unsigned long default_root_window;
	unsigned long n_items, bytes_after_ret;
	int actual_format;
	Window *props;
	Atom atom_type_prop;

	dpy = XOpenDisplay(0);
	CHECK(!dpy);

	default_root_window = DefaultRootWindow(dpy);

	atom = XListProperties(dpy, default_root_window, &natoms);
	CHECK(!atom);
	printf("Number of properties = %d\n", natoms);
	for (; i < natoms; ++i) {
		property = XGetAtomName(dpy, atom[i]);
		printf("property[%d] = %s\n", i, property);

		CHECK(XGetWindowProperty(dpy, default_root_window, atom[i], 0,
		 -1, False, AnyPropertyType, &atom_type_prop, &actual_format,
		 &n_items, &bytes_after_ret, (unsigned char **)&props));
		puts(props ? (char *)props : "NULL");
		XFree(props);
		XFree(property);
	}

	XFree(atom);
	XCloseDisplay(dpy);
}
