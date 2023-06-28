#include <gtk-3.0/gtk/gtk.h>

int
main(int argc, char *argv[])
{

	GtkWidget *window;
	GtkWidget *dialog;

	gtk_init(&argc, &argv);

	dialog = gtk_message_dialog_new(0, 0, 0, 0,
	 "This is stupid\n"
	 "This is stupid");

	g_signal_connect_swapped(GTK_OBJECT(dialog), "response",
	 G_CALLBACK(gtk_widget_destroy), GTK_OBJECT(dialog));

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	/* gtk_widget_show(window); */

	/* g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	 */

	/* gtk_main(); */

	return 0;
}
