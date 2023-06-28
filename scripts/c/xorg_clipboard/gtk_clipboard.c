#include <gtk-3.0/gtk/gtk.h>

int
main(const int argc, const char *const *const argv)
{
	GtkClipboard *clipboard;

	gtk_init(0, 0);

	clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	if (!clipboard) {
		return 1;
	}

	switch (argc) {
	case 1: {
		gchar *text;
		text = gtk_clipboard_wait_for_text(clipboard);
		if (!text) {
			return 1;
		}
		puts(text);
		break;
	}
	case 2: {
		gtk_clipboard_set_text(clipboard, argv[1], -1);
		gtk_clipboard_store(clipboard);

		/*
				clipboard =
		   gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
				gtk_clipboard_set_text(clipboard, argv[1], -1);
		*/
		break;
	}
	default:
		return 1;
	}

	/* No point in freeing since the program is exiting. */
	/* g_free(text); */
	return 0;
}
