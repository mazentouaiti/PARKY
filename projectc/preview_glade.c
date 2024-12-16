#include <gtk/gtk.h>
#include <glade/glade.h>

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Load the Glade file
    GladeXML *xml = glade_xml_new("projectc.glade", NULL, NULL);
    if (!xml) {
        g_print("Failed to load Glade file.\n");
        return 1;
    }

    // Get the main window (replace "window1" with your window's ID)
    GtkWidget *window = glade_xml_get_widget(xml, "user_interface");
    if (!window) {
        g_print("Main window not found in Glade file.\n");
        return 1;
    }

    // Connect signals and show the window
    glade_xml_signal_autoconnect(xml);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

