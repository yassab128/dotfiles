//true; exec ccompile "$0" -lX11 -- "$@"

#include <X11/Xlib.h>

static void set_status(Display *display, char *s)
{
	Window rootWindow = DefaultRootWindow(display);
	XStoreName(display, rootWindow, s);
	XSync(display, False);
}

int main()
{
	Display *display = XOpenDisplay(0);
	if (!display) {
		return 1;
	}
	set_status(display, " ");
	XCloseDisplay(display);
}
