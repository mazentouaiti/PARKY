#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtkctree.h>
#include <gtk/gtk.h>
#include "operation.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "global.h"
#include "parking.h"
#include "worker.h"
#include "login.h"

int logged_in_citizen_id = -1;



void
on_cancel_btn_edit_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_apply_btn_edit_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    const char *filename = "citizen_profile.txt"; 
    citizen current_citizen;

    // Read the current citizen profile
    if (read_citizen_profile(filename, &current_citizen) != 0) {
        g_print("Error reading citizen profile.\n");
        return;
    }

    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *entry_name;
	GtkWidget *entry_surname;
	GtkWidget *entry_email;
	GtkWidget *entry_password;
	GtkWidget *entry_car_brand;
	GtkWidget *entry_car_serial;
	GtkWidget *entry_color;  
        GtkWidget *radio_male;  
        GtkWidget *radio_female;
        GtkWidget *file_selector;

	entry_name = lookup_widget(parent_window, "edit_name");
	entry_surname = lookup_widget(parent_window, "edit_surname");
	entry_email = lookup_widget(parent_window, "edit_email");
	entry_password = lookup_widget(parent_window, "edit_passwd");
	entry_car_brand = lookup_widget(parent_window, "edit_carbrand");
	entry_car_serial = lookup_widget(parent_window, "edit_serial");
	entry_color = lookup_widget(parent_window, "edit_color");
        radio_male = lookup_widget(parent_window, "radiomale");
        radio_female = lookup_widget(parent_window, "radiofemale");
        file_selector = lookup_widget(parent_window, "filechooserpdp");

    // Ensure all widgets are found
    if (!entry_name || !entry_surname || !entry_email || !entry_password ||
        !entry_car_brand || !entry_car_serial || !entry_color || 
        !radio_male || !radio_female || !file_selector) {
        g_print("Error: Some widgets not found!\n");
        return;
    }
    // Get the profile picture file path from the file selector (if available)
    const gchar *pdp_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selector));

    // Get updated values from user input
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const gchar *surname = gtk_entry_get_text(GTK_ENTRY(entry_surname));
    const gchar *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const gchar *carbrand = gtk_entry_get_text(GTK_ENTRY(entry_car_brand));
    const gchar *serial = gtk_entry_get_text(GTK_ENTRY(entry_car_serial));
    const gchar *color = gtk_entry_get_text(GTK_ENTRY(entry_color));
    
    // Get gender from radio buttons
    const gchar *gender = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_male)) ? 
                          "Male" : 
                          (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_female)) ? "Female" : "");

    if (strlen(gender) == 0) {
        g_print("Error: Gender not selected.\n");
        return;
    }

    // Update citizen struct with new values
    strncpy(current_citizen.name, name, sizeof(current_citizen.name) - 1);
    strncpy(current_citizen.surname, surname, sizeof(current_citizen.surname) - 1);
    strncpy(current_citizen.email, email, sizeof(current_citizen.email) - 1);
    strncpy(current_citizen.password, password, sizeof(current_citizen.password) - 1);
    strncpy(current_citizen.car_brand, carbrand, sizeof(current_citizen.car_brand) - 1);
    strncpy(current_citizen.car_serial, serial, sizeof(current_citizen.car_serial) - 1);
    strncpy(current_citizen.color, color, sizeof(current_citizen.color) - 1);
    strncpy(current_citizen.gender, gender, sizeof(current_citizen.gender) - 1);
 if (pdp_filename != NULL) {
        strncpy(current_citizen.profile_picture, pdp_filename, sizeof(current_citizen.profile_picture) - 1);
    }
    // Save updated profile back to the file
    if (update_citizen_profile(filename, &current_citizen) == 0) {
        g_print("Profile updated successfully.\n");
    } else {
        g_print("Error updating citizen profile.\n");
    }
}




void
on_delete_profile_btn_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *delete_warning;
delete_warning = create_delete_warning ();
  gtk_widget_show (delete_warning);
}


void on_update_btn_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *treeview; 
    GtkWidget *textview;
    
    // Lookup widgets
    treeview = lookup_widget(GTK_WIDGET(button), "treeview_invoice");
    textview = lookup_widget(GTK_WIDGET(button), "textview_invoice");
    
    if (!treeview || !textview) {
        g_print("Error: Missing treeview or textview widgets.\n");
        return;
    }

    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Ensure columns are set up only once
        setup_tree_view(GTK_TREE_VIEW(treeview));
        columns_added = TRUE;
    }

    // Create a new GtkListStore with 3 columns
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT);
    GtkTreeIter iter;

    // Array to hold parking data
    parking_data data_array[100];
    int count = read_parking_data("invoice.txt", data_array, 100);
    if (count < 0) {
        g_print("Error: Unable to read parking data.\n");
        return;
    }

    // Populate the GtkListStore and calculate the total cost
    float total_cost = 0;
    for (int i = 0; i < count; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           0, data_array[i].duration,  // Column 0
                           1, data_array[i].date,     // Column 1
                           2, data_array[i].cost,     // Column 2
                           -1);

        total_cost += data_array[i].cost;
    }

    // Set the model for the treeview
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the reference to the model

    // Display the total cost in the GtkTextView
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gchar *total_cost_str = g_strdup_printf("%.2f DTN", total_cost);
    gtk_text_buffer_set_text(buffer, total_cost_str, -1);
    g_free(total_cost_str);
}

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

// Get the ID entry widget (assume it's global or accessible)
    GtkWidget *id_entry;
     id_entry= lookup_widget((GtkWidget *)button, "id_delete");

    // Get the text from the entry
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(id_entry));
    int id_to_delete = atoi(id_text); // Convert to integer

    const char *filename = "citizen_profile.txt";
    int delete_result = delete_citizen_profile(filename, id_to_delete);

    if (delete_result == 0) {
        g_print("Citizen profile with ID %d deleted successfully.\n", id_to_delete);
    } else if (delete_result == -2) {
        g_print("Citizen profile with ID %d not found.\n", id_to_delete);
    } else {
        g_print("An error occurred while deleting the citizen profile.\n");
    }
}



void
on_btn_print_fact_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *invoice;
    invoice = create_invoice ();    
    gtk_widget_show(invoice);
}


void
on_togglebutton2_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_togglebutton1_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button12_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button13_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_helpbutton1_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *delete_warning;
  // Assuming 'login' is a top-level window
    delete_warning = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(delete_warning)) {
        gtk_widget_hide(delete_warning); // Hide the login window
        g_print("delete_warning window is now hidden.\n");
    } else {
        g_warning("The 'delete_warning' widget is not a valid window.");
    }
}





void
on_cancelbutton2_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_okbutton2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *user_interface;
    GtkWidget *logout;
    
    user_interface = gtk_widget_get_toplevel(GTK_WIDGET(button));
    
    
    if (GTK_IS_WINDOW(user_interface)) {
        gtk_widget_hide(user_interface);
         
        g_print(" window user is now hidden.\n");
    } else {
        g_warning("The widget is not a valid window.");
    }
   /*logout = gtk_widget_get_toplevel(GTK_WIDGET(button));
   if (GTK_IS_WINDOW(logout)) {
        
        gtk_widget_hide(logout); 
        g_print(" window logout is now hidden.\n");
    } else {
        g_warning("The widget is not a valid window.");
    }*/
}


void
on_cancelbutton3_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

GtkWidget *invoice;
  // Assuming 'login' is a top-level window
    invoice = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(invoice)) {
        gtk_widget_hide(invoice); // Hide the login window
        g_print("invoice window is now hidden.\n");
    } else {
        g_warning("The 'invoice' widget is not a valid window.");
    }
}


void
on_okbutton3_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_user_interface_show                 (GtkWidget       *widget,
                                        gpointer         user_data)
{

}


void
on_btn_show_profile_data_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *name_textview;
    GtkWidget *surname_textview;
    GtkWidget *email_textview;
    GtkWidget *carbrand_textview;
    GtkWidget *serial_textview;
    GtkWidget *password_entry;
    GtkWidget *color_textview;
    GtkWidget *male_checkbox;
    GtkWidget *female_checkbox;
    GtkWidget *image_widget;

    // Lookup widgets
    name_textview = lookup_widget(parent_window, "textview_name");
    surname_textview = lookup_widget(parent_window, "surname_textview");
    email_textview = lookup_widget(parent_window, "textview_email");
    carbrand_textview = lookup_widget(parent_window, "textview_car_brand");
    serial_textview = lookup_widget(parent_window, "textview_serial");
    password_entry = lookup_widget(parent_window, "entry_passwrd");
    color_textview = lookup_widget(parent_window, "textview_color");
    male_checkbox = lookup_widget(parent_window, "checkmale");
    female_checkbox = lookup_widget(parent_window, "checkfemale");
    image_widget = lookup_widget(parent_window, "pdp");

    // Ensure all widgets are found
    if (!name_textview || !surname_textview || !email_textview || !carbrand_textview || 
        !serial_textview || !password_entry || !color_textview || 
        !male_checkbox || !female_checkbox || !image_widget) {
        g_print("Error: Some widgets not found!\n");
        return;
    }

     // Read citizen profile from file using the logged-in citizen ID
    citizen citizen_data;
    if (logged_in_citizen_id < 0) {
        g_print("Error: Invalid logged-in citizen ID.\n");
        return;
    }

    int result = read_citizen_profile_by_id("citizen_profile.txt", logged_in_citizen_id, &citizen_data);
    if (result == 0) {
        g_print("Error: No citizen found with the provided ID: %d\n", logged_in_citizen_id);
        return;
    }

    // Display data in respective widgets
    GtkTextBuffer *buffer;

    // Name
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(name_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.name, -1);

    // Surname
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(surname_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.surname, -1);

    // Email
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(email_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.email, -1);

    // Password
    gtk_entry_set_text(GTK_ENTRY(password_entry), citizen_data.password);

    // Car Brand
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(carbrand_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.car_brand, -1);

    // Serial
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(serial_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.car_serial, -1);

    // Color
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(color_textview));
    gtk_text_buffer_set_text(buffer, citizen_data.color, -1);

    // Gender: Set checkboxes based on gender value
    if (strcmp(citizen_data.gender, "Male") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(male_checkbox), TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(female_checkbox), FALSE);
    } else if (strcmp(citizen_data.gender, "Female") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(male_checkbox), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(female_checkbox), TRUE);
    } else {
        // Default: Uncheck both if gender is unknown
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(male_checkbox), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(female_checkbox), FALSE);
    }

    // Profile picture
    if (citizen_data.profile_picture[0] != '\0') {
        gtk_image_set_from_file(GTK_IMAGE(image_widget), citizen_data.profile_picture);
    } else {
        // If no profile picture, set a default image
        gtk_image_set_from_stock(GTK_IMAGE(image_widget), GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_DIALOG);
    }

    g_print("Citizen profile displayed successfully.\n");
}




void
on_reserv_btn_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_feedback_btn_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btn_logout_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  
  GtkWidget *login;
  GtkWidget *user_interface;
  login = create_login ();
  gtk_widget_show (login);

// Assuming 'login' is a top-level window
    user_interface = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(user_interface)) {
        gtk_widget_hide(user_interface); // Hide the login window
        g_print("user_interface window is now hidden.\n");
    } else {
        g_warning("The 'user_interface' widget is not a valid window.");
    }
logged_in_citizen_id = -1;
  
}

void setup_tree_view(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Remove all existing columns from the TreeView
    GList *columns = gtk_tree_view_get_columns(treeview);
    for (GList *iter = columns; iter != NULL; iter = iter->next) {
        gtk_tree_view_remove_column(treeview, GTK_TREE_VIEW_COLUMN(iter->data));
    }
    g_list_free(columns);

    // Column 1: Duration

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Duration", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Column 2: Date
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Date", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Column 3: Cost
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Cost", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(treeview, column);
}





void
on_radiomale_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_radiofemale_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_Addbtn_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_signinbtn_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *addwindow;
    GtkWidget *login;
    const char *filename = "citizen_profile.txt"; 
    citizen new_citizen;

	GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	GtkWidget *entry_id;
	GtkWidget *entry_name;
	GtkWidget *entry_surname;
	GtkWidget *entry_email;
	GtkWidget *entry_password;
	GtkWidget *entry_car_brand;
	GtkWidget *entry_car_serial;
	GtkWidget *entry_color;
	GtkWidget *radio_male;
	GtkWidget *radio_female;
        GtkWidget *file_selector;

    // Retrieve widget pointers
    entry_id = lookup_widget(parent_window, "idadd");
    entry_name = lookup_widget(parent_window, "nameadd");
    entry_surname = lookup_widget(parent_window, "surnameadd");
    entry_email = lookup_widget(parent_window, "emailadd");
    entry_password = lookup_widget(parent_window, "passadd");
    entry_car_brand = lookup_widget(parent_window, "brandadd");
    entry_car_serial = lookup_widget(parent_window, "serialadd");
    entry_color = lookup_widget(parent_window, "coloradd");
    radio_male = lookup_widget(parent_window, "radiomaleadd");
    radio_female = lookup_widget(parent_window, "radiofemaleadd");
    file_selector = lookup_widget(parent_window, "filechooseradd");
    

    // Validate widget pointers
    if (!entry_id || !entry_name || !entry_surname || !entry_email || !entry_password ||
        !entry_car_brand || !entry_car_serial || !entry_color || !radio_male || !radio_female || !file_selector) {
        g_print("Error: Some widgets could not be found.\n");
        return;
    }
 const gchar *pdp_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_selector));
    // Get user input
    const char *id = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const gchar *surname = gtk_entry_get_text(GTK_ENTRY(entry_surname));
    const gchar *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const gchar *car_brand = gtk_entry_get_text(GTK_ENTRY(entry_car_brand));
    const gchar *car_serial = gtk_entry_get_text(GTK_ENTRY(entry_car_serial));
    const gchar *color = gtk_entry_get_text(GTK_ENTRY(entry_color));
    const gchar *gender = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_male)) ? 
                          "Male" : 
                          (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_female)) ? "Female" : "");

    

    // Validate user input
    if (strlen(id) == 0 || strlen(name) == 0 || strlen(surname) == 0 || strlen(email) == 0 || 
        strlen(password) == 0 || strlen(car_brand) == 0 || strlen(car_serial) == 0 || strlen(color) == 0 || 
        strlen(gender) == 0) {
        g_print("Error: All fields must be filled out.\n");
        return ;
    }

    int cin=atoi(id);
    
    // Fill the citizen struct
    new_citizen.id=cin;
    strncpy(new_citizen.name, name, sizeof(new_citizen.name) - 1);
    strncpy(new_citizen.surname, surname, sizeof(new_citizen.surname) - 1);
    strncpy(new_citizen.email, email, sizeof(new_citizen.email) - 1);
    strncpy(new_citizen.password, password, sizeof(new_citizen.password) - 1);
    strncpy(new_citizen.car_brand, car_brand, sizeof(new_citizen.car_brand) - 1);
    strncpy(new_citizen.car_serial, car_serial, sizeof(new_citizen.car_serial) - 1);
    strncpy(new_citizen.color, color, sizeof(new_citizen.color) - 1);
    strncpy(new_citizen.gender, gender, sizeof(new_citizen.gender) - 1);
   if (pdp_filename != NULL) {
        strncpy(new_citizen.profile_picture, pdp_filename, sizeof(new_citizen.profile_picture) - 1);
    }
    
// Save citizen to file
    if (add_citizen(filename, &new_citizen) != 1) {
        g_print("Error: Could not add to the file.\n");
        return;
    }
    g_print("Citizen profile added successfully.\n");
   

    // Window management
    login = create_login();
    gtk_widget_show(login);

    addwindow = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(addwindow)) {
        gtk_widget_hide(addwindow);
    } else {
        g_warning("Error: addwindow is not a valid window.");
    }
}



void
on_sinupbtn_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *addwindow;
GtkWidget *login;
addwindow = create_addwindow ();
  gtk_widget_show (addwindow);



  // Assuming 'login' is a top-level window
    login = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(login)) {
        gtk_widget_hide(login); // Hide the login window
        g_print("Login window is now hidden.\n");
    } else {
        g_warning("The 'login' widget is not a valid window.");
    }
}


void
on_loginbtn_clicked(GtkButton *button, gpointer user_data) 
{
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *entry_id = lookup_widget(parent_window, "idlogin");
    GtkWidget *entry_password = lookup_widget(parent_window, "passlogin");

    if (!entry_id || !entry_password) {
        g_print("Error: Widgets for ID and password not found.\n");
        return;
    }

    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const gchar *password_text = gtk_entry_get_text(GTK_ENTRY(entry_password));

    // Validate user input
    if (strlen(id_text) == 0 || strlen(password_text) == 0) {
        g_print("Error: ID and password fields must not be empty.\n");
        return;
    }

    // Convert ID to integer
    int id = atoi(id_text);
    if (id == 0 && strcmp(id_text, "0") != 0) {
        g_print("Error: Invalid ID format.\n");
        return;
    }

    const char *filename = "citizen_profile.txt";
    citizen citizen_data_array[1]; // Create an array to hold the results (one citizen)
    int max_results = 1; // We expect only one result, since IDs should be unique

    // Search for citizen by ID
    int result = search_all_citizens_by_id(filename, id, citizen_data_array, max_results);
    if (result == -1) {
        g_print("Error: Unable to open the citizen file.\n");
        return;
    } else if (result == 0) {
        g_print("Error: No citizen found with the provided ID.\n");
        return;
    }else{logged_in_citizen_id = id;}

    // Verify password
    if (strcmp(citizen_data_array[0].password, password_text) == 0) {
        g_print("Login successful. Welcome, %s %s!\n", citizen_data_array[0].name, citizen_data_array[0].surname);

        // Save the logged-in ID globally
        logged_in_citizen_id = id;

        // Create and show the new user interface window
        GtkWidget *user_interface = create_user_interface();
        gtk_widget_show(user_interface);

        // Hide the login window
        GtkWidget *login = gtk_widget_get_toplevel(GTK_WIDGET(button));
        if (GTK_IS_WINDOW(login)) {
            gtk_widget_hide(login);
        } else {
            g_warning("Error: login_window is not a valid window.");
        }
    } else {
        g_print("Error: Incorrect password.\n");
    }
}



void
on_adminbtn_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *AdminwindowLogin , *welcome;
AdminwindowLogin = create_AdminwindowLogin ();
  gtk_widget_show (AdminwindowLogin);

    welcome = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(welcome)) {
        gtk_widget_hide(welcome); // Hide the login window
        g_print("welcome window is now hidden.\n");
    } else {
        g_warning("The 'window' widget is not a valid window.");
    }
}


void
on_clientbtn_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *login , *welcome;
login = create_login ();
  gtk_widget_show (login);

    welcome = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(welcome)) {
        gtk_widget_hide(welcome); // Hide the login window
        g_print("welcome window is now hidden.\n");
    } else {
        g_warning("The 'window' widget is not a valid window.");
    }
}


void
on_backaddtologin_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *addwindow , *login;
login = create_login ();
  gtk_widget_show (login);

    addwindow = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(addwindow)) {
        gtk_widget_hide(addwindow); // Hide the login window
        g_print("addwindow is now hidden.\n");
    } else {
        g_warning("The 'addwindow' widget is not a valid window.");
    }
}


void
on_backlogintowelcome_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *login , *welcome;
welcome = create_welcome ();
  gtk_widget_show (welcome);

    login = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(login)) {
        gtk_widget_hide(login); // Hide the login window
        g_print("login window is now hidden.\n");
    } else {
        g_warning("The login widget is not a valid window.");
    }

}

void
on_MGAdminRemovebutton_clicked         (GtkButton       *button1,
                                        gpointer         user_data)
{


    
    // Retrieve widgets
    GtkWidget *park_id_entry = lookup_widget(GTK_WIDGET(button1), "MGAdminparkidentry");
    GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button1), "MGlabelfeedback");
    const char *park_id = gtk_entry_get_text(GTK_ENTRY(park_id_entry));

    // Check if Park ID is empty
    if (strlen(park_id) == 0) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Park ID cannot be empty!");
        return;
    }

    // Confirmation dialog
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button1)));
    GtkWidget *confirmation_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to delete this parking configuration?"
    );

    gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
    gtk_widget_destroy(confirmation_dialog);

    if (response == GTK_RESPONSE_YES) {
        int test = remove_parking("parkdata.txt", park_id);

        if (test == 1) {
            gtk_label_set_text(GTK_LABEL(feedback_label), "Parking configuration removed successfully!");
            parkingremoved("workerdata.txt", park_id);

            // Success dialog
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(parent_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Parking configuration deleted successfully."
            );
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            gtk_label_set_text(GTK_LABEL(feedback_label), "Park ID not found!");
        }

        // Update tree views
        show_tree_view(GTK_WIDGET(button1));
        show_worker_tree_view(GTK_WIDGET(button1));
    }
}

void
on_MGAdminAddbutton_clicked            (GtkButton       *button2,
                                        gpointer         user_data)
{

    GtkWidget *park_id_entry = lookup_widget(GTK_WIDGET(button2), "MGAdminparkidentry");
    GtkWidget *status_combo = lookup_widget(GTK_WIDGET(button2), "MGAdminStatuscombo");
    GtkWidget *size_combo = lookup_widget(GTK_WIDGET(button2), "MGAdminSizecombo");
    GtkWidget *location_entry = lookup_widget(GTK_WIDGET(button2), "MGAdminLocationentry");
    GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button2), "MGlabelfeedback");

    const char *park_id = gtk_entry_get_text(GTK_ENTRY(park_id_entry));
    const char *status = gtk_combo_box_get_active_text(GTK_COMBO_BOX(status_combo));
    const char *size = gtk_combo_box_get_active_text(GTK_COMBO_BOX(size_combo));
    const char *location = gtk_entry_get_text(GTK_ENTRY(location_entry));

    // check
    if (status == NULL || size == NULL || strlen(location) == 0) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Error: All fields must be filled.");
        return;
    }
    // Confirmation dialog
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button2)));
    GtkWidget *confirmation_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to Add this parking configuration?"
    );

    gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
    gtk_widget_destroy(confirmation_dialog);
    if (response == GTK_RESPONSE_YES) {  
       // Create a Parking structure and populate it
       Parking parking;
       strcpy(parking.park_id, park_id);
       strcpy(parking.status, status);
       strcpy(parking.size, size);
       strcpy(parking.location, location); 
       // Write the Parking data to the file
       write_parking_to_file("parkdata.txt", parking);
       // Set success feedback
       gtk_label_set_text(GTK_LABEL(feedback_label), "Parking configuration added successfully!");
       // Success dialog
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(parent_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Parking configuration added successfully."
            );
       // Clear inputs 
       gtk_entry_set_text(GTK_ENTRY(park_id_entry), "");
       gtk_combo_box_set_active(GTK_COMBO_BOX(status_combo), -1);
       gtk_combo_box_set_active(GTK_COMBO_BOX(size_combo), -1);
       gtk_entry_set_text(GTK_ENTRY(location_entry), "");
       // Refresh the TreeView
       show_tree_view(GTK_WIDGET(button2));
   }
}


void
on_MGAdminModifybutton_clicked         (GtkButton       *button3,
                                        gpointer         user_data)
{
  

    // Retrieve inputs
    GtkWidget *park_id_entry = lookup_widget(GTK_WIDGET(button3), "MGAdminparkidentry");
    GtkWidget *status_combo = lookup_widget(GTK_WIDGET(button3), "MGAdminStatuscombo");
    GtkWidget *size_combo = lookup_widget(GTK_WIDGET(button3), "MGAdminSizecombo");
    GtkWidget *location_entry = lookup_widget(GTK_WIDGET(button3), "MGAdminLocationentry");
    GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button3), "MGlabelfeedback");

    const char *park_id = gtk_entry_get_text(GTK_ENTRY(park_id_entry));
    const char *status = gtk_combo_box_get_active_text(GTK_COMBO_BOX(status_combo));
    const char *size = gtk_combo_box_get_active_text(GTK_COMBO_BOX(size_combo));
    const char *location = gtk_entry_get_text(GTK_ENTRY(location_entry));
    char feedback_message[100];
    int test;

    if (strlen(park_id) == 0 || status == NULL || size == NULL || strlen(location) == 0) {
        gtk_label_set_text(GTK_LABEL(feedback_label),"Error: All fields must be filled.");
        return;
    }
    // Confirmation dialog
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button3)));
    GtkWidget *confirmation_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to apply this parking modification?"
    );
    gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
    gtk_widget_destroy(confirmation_dialog);
    if (response == GTK_RESPONSE_YES) {
      test=modify_parking("parkdata.txt", park_id, status, size, location);
      if (test==0)
      gtk_label_set_text(GTK_LABEL(feedback_label), "Parking configuration modifyed successfully!");
      // Success dialog
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(parent_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Parking configuration deleted successfully."
            );
      if (test==1)
      gtk_label_set_text(GTK_LABEL(feedback_label), "parkID not found");
      // Clear inputs 
       gtk_entry_set_text(GTK_ENTRY(park_id_entry), "");
       gtk_combo_box_set_active(GTK_COMBO_BOX(status_combo), -1);
       gtk_combo_box_set_active(GTK_COMBO_BOX(size_combo), -1);
       gtk_entry_set_text(GTK_ENTRY(location_entry), "");
      // Refresh the TreeView
      show_tree_view(GTK_WIDGET(button3));

    }
}
void show_tree_view(GtkWidget *button) {
    // Use lookup_widget to find the tree view widget
    GtkWidget *tree_view = lookup_widget(button, "MGAdminTreeView");

    if (!tree_view) {
        g_printerr("Error: TreeView widget not found.\n");
        return;
    }

    // Create the ListStore (4 columns: Park ID, Status, Size, Location)
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Open the file to read the parking data
    FILE *file = fopen("parkdata.txt", "r");
    if (!file) {
        g_printerr("Error opening parkdata.txt.\n");
        return;
    }

    char park_id[20], status[20], size[20], location[50];
    while (fscanf(file, "%19s %19s %19s %49s", park_id, status, size, location) == 4) {
        // Create a new iterator for inserting data into the ListStore
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);

        // Set the data for each column
        gtk_list_store_set(store, &iter,0, park_id,1, status,2, size,3, location,-1);              
    }
    fclose(file);

    // Add columns to the TreeView if no columns exist
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(tree_view)) == NULL) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        //  (first column)
        GtkTreeViewColumn *col_park_id = gtk_tree_view_column_new_with_attributes("Park ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_park_id);

        // (second column)
        GtkTreeViewColumn *col_status = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_status);

        //  (third column)
        GtkTreeViewColumn *col_size = gtk_tree_view_column_new_with_attributes("Size", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_size);

        //  (fourth column)
        GtkTreeViewColumn *col_location = gtk_tree_view_column_new_with_attributes("Location", renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_location);
    }

    // Set the ListStore as the model for the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    //  avoid memory leaks
    g_object_unref(store);
}
void
on_AdminwindowParkConfig_show          (GtkWidget       *widget,
                                        gpointer         user_data)
{
show_worker_tree_view(GTK_WIDGET(widget));
show_tree_view(GTK_WIDGET(widget));
}


void
on_MGadminworkerUnassign_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *worker_id_entry = lookup_widget(GTK_WIDGET(button), "MGAdminworkeridentry");
    GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button), "feedback2");

    // Get user input
    const char *worker_id = gtk_entry_get_text(GTK_ENTRY(worker_id_entry));

    // Validate input
    if (strlen(worker_id) == 0) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Error: Worker ID must be filled.");
        return;
    }
    // Confirmation dialog
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *confirmation_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to Unassign this worker?"
    );
    gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
    gtk_widget_destroy(confirmation_dialog);
    if (response == GTK_RESPONSE_YES) {
    // Unassign the worker
    if (unassign_worker("workerdata.txt", worker_id)) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Worker successfully unassigned.");
        // Success dialog
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(parent_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Working unassign successfully."
            );
    } else {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Error: Worker ID does not exist or unassignment failed.");
    }

    // Refresh the TreeView 
    show_worker_tree_view(GTK_WIDGET(button));
 }
}


void
on_MGadminworkerAssign_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
// Lookup input widgets
    GtkWidget *worker_id_entry = lookup_widget(GTK_WIDGET(button), "MGAdminworkeridentry");
    GtkWidget *parking_id_entry = lookup_widget(GTK_WIDGET(button), "MGAdminparkidworkerentry");
    GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button), "feedback2");

    // Get user input
    const char *worker_id = gtk_entry_get_text(GTK_ENTRY(worker_id_entry));
    const char *parking_id = gtk_entry_get_text(GTK_ENTRY(parking_id_entry));

    // Validate inputs
    if (strlen(worker_id) == 0 || strlen(parking_id) == 0) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Error: Both Worker ID and Parking ID must be filled.");
        return;
    }

    // Check if parking ID exists in parkdata.txt
    if (!is_park_id_exist("parkdata.txt", parking_id)) {
        char error_message[100];
        snprintf(error_message, sizeof(error_message), "Error: Parking ID '%s' does not exist.", parking_id);
        gtk_label_set_text(GTK_LABEL(feedback_label), error_message);
        return;
    }
    // Confirmation dialog
    GtkWidget *parent_window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *confirmation_dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to assign this worker?"
    );
    gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
    gtk_widget_destroy(confirmation_dialog);
    if (response == GTK_RESPONSE_YES) {
    // Assign the worker to the parking ID
    if (assign_worker_to_parking("workerdata.txt", worker_id, parking_id)) {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Worker successfully assigned to the parking ID.");
        // Success dialog
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(parent_window),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Working assigned successfully."
            );
    } else {
        gtk_label_set_text(GTK_LABEL(feedback_label), "Error: Worker ID does not exist or assignment failed.");
    }

    // Refresh the TreeView to reflect updated data
    show_worker_tree_view(GTK_WIDGET(button));
  }
}
void show_worker_tree_view(GtkWidget *button) {
    // Lookup the TreeView widget using the parent widget
    GtkWidget *tree_view = lookup_widget(button, "MGtreeviewWorker");

    if (!tree_view) {
        g_printerr("Error: Worker TreeView widget not found.\n");
        return;
    }

    // Create the ListStore with 3 columns (worker_id, worker_name, worker_park)
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Read worker data from file
    FILE *file = fopen("workerdata.txt", "r");
    if (!file) {
        g_printerr("Error: Could not open workerdata.txt.\n");
        return;
    }

    worker w;
    while (fscanf(file, "%19s %49s %49s", w.workerid, w.workername, w.workerparkid) == 3) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, w.workerid, 1, w.workername, 2, w.workerparkid, -1);
    }

    fclose(file);

    // Add columns to the TreeView if no columns exist
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(tree_view)) == NULL) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        //  (first column)
        GtkTreeViewColumn *col_worker_id = gtk_tree_view_column_new_with_attributes("Worker ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_id);

        //  (second column)
        GtkTreeViewColumn *col_worker_name = gtk_tree_view_column_new_with_attributes("Worker Name", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_name);

        //  (third column)
        GtkTreeViewColumn *col_worker_park = gtk_tree_view_column_new_with_attributes("Worker Park ID", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_park);
    }

    // Set the ListStore as the model for the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    //  avoid memory leaks
    g_object_unref(store);
}
void show_tree_view_search(GtkWidget *button) {
    // Use lookup_widget to find the tree view widget
    GtkWidget *tree_view = lookup_widget(button, "MGAdminTreeView");

    if (!tree_view) {
        g_printerr("Error: TreeView widget not found.\n");
        return;
    }

    // Create the ListStore (4 columns: Park ID, Status, Size, Location)
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Open the file to read the parking data
    FILE *file = fopen("searchpark.txt", "r");
    if (!file) {
        g_printerr("Error opening parkdata.txt.\n");
        return;
    }

    char park_id[20], status[20], size[20], location[50];
    while (fscanf(file, "%19s %19s %19s %49s", park_id, status, size, location) == 4) {
        // Create a new iterator for inserting data into the ListStore
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);

        // Set the data for each column
        gtk_list_store_set(store, &iter,0, park_id,1, status,2, size,3, location,-1);              
    }
    fclose(file);

    // Add columns to the TreeView if no columns exist
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(tree_view)) == NULL) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        //  (first column)
        GtkTreeViewColumn *col_park_id = gtk_tree_view_column_new_with_attributes("Park ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_park_id);

        // (second column)
        GtkTreeViewColumn *col_status = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_status);

        //  (third column)
        GtkTreeViewColumn *col_size = gtk_tree_view_column_new_with_attributes("Size", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_size);

        //  (fourth column)
        GtkTreeViewColumn *col_location = gtk_tree_view_column_new_with_attributes("Location", renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_location);
    }

    // Set the ListStore as the model for the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    //  avoid memory leaks
    g_object_unref(store);
}

void show_workersearch_tree_view(GtkWidget *button) {
    // Lookup the TreeView widget using the parent widget
    GtkWidget *tree_view = lookup_widget(button, "MGtreeviewWorker");

    if (!tree_view) {
        g_printerr("Error: Worker TreeView widget not found.\n");
        return;
    }

    // Create the ListStore with 3 columns (worker_id, worker_name, worker_park)
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // Read worker data from file
    FILE *file = fopen("searchworker.txt", "r");
    if (!file) {
        g_printerr("Error: Could not open workerdata.txt.\n");
        return;
    }

    worker w;
    while (fscanf(file, "%19s %49s %49s", w.workerid, w.workername, w.workerparkid) == 3) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, w.workerid, 1, w.workername, 2, w.workerparkid, -1);
    }

    fclose(file);

    // Add columns to the TreeView if no columns exist
    if (gtk_tree_view_get_columns(GTK_TREE_VIEW(tree_view)) == NULL) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

        //  (first column)
        GtkTreeViewColumn *col_worker_id = gtk_tree_view_column_new_with_attributes("Worker ID", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_id);

        //  (second column)
        GtkTreeViewColumn *col_worker_name = gtk_tree_view_column_new_with_attributes("Worker Name", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_name);

        //  (third column)
        GtkTreeViewColumn *col_worker_park = gtk_tree_view_column_new_with_attributes("Worker Park ID", renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_worker_park);
    }

    // Set the ListStore as the model for the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    //  avoid memory leaks
    g_object_unref(store);
}


int valeur;

void
on_MGadminradiobuttonmale_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
valeur=2;
}


void
on_MGadminradiobuttonfemale_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
valeur=1;
}
void
on_MGCreataccbutton_clicked            (GtkButton       *button4,
                                        gpointer         user_data)
{   utilisateur u;
    char msg[10]; // Buffer for sex string

    // Retrieve GTK widgets
    GtkWidget *entryname = lookup_widget(GTK_WIDGET(button4), "MGAdminNamenewaccentry");
    GtkWidget *entrysurname = lookup_widget(GTK_WIDGET(button4), "MGAdminSurnameentry");
    GtkWidget *entrylogin = lookup_widget(GTK_WIDGET(button4), "MGAdminLoginaccentry");
    GtkWidget *entrypass = lookup_widget(GTK_WIDGET(button4), "MGAdminPasswordentry");
    GtkWidget *entrymail = lookup_widget(GTK_WIDGET(button4), "MGadminentryEmailnewacc");
    GtkWidget *feedback_label2 = lookup_widget(GTK_WIDGET(button4), "feedbacklogin");

    // Validate widget lookups
    if (!entryname || !entrysurname || !entrylogin || !entrypass || !entrymail) {
        g_print("Error: Failed to retrieve one or more input widgets.\n");
        
        return;
    }

    // Get text from entry fields
    const char *name = gtk_entry_get_text(GTK_ENTRY(entryname));
    const char *surname = gtk_entry_get_text(GTK_ENTRY(entrysurname));
    const char *login = gtk_entry_get_text(GTK_ENTRY(entrylogin));
    const char *pass = gtk_entry_get_text(GTK_ENTRY(entrypass));
    const char *mail = gtk_entry_get_text(GTK_ENTRY(entrymail));

    
    
     if (strlen(name) == 0 || strlen(surname) == 0 || strlen(login) == 0 || strlen(pass) == 0 || strlen(mail) == 0) {
        g_print("Error: All fields must be filled out.\n");
        gtk_label_set_text(GTK_LABEL(feedback_label2),"Error: All fields must be filled out.");
        return; // Stop execution if any field is empty
    }

    // Simulate a value for sex (in a real application, this would be provided by user input)
    sex(valeur, msg);

    // Populate utilisateur structure
    strcpy(u.name, name);
    strcpy(u.surname, surname);
    strcpy(u.login, login);
    strcpy(u.pass, pass); // Consider hashing the password for security
    strcpy(u.mail, mail);
    strcpy(u.sex, msg);

    // Create the account and save it to a file
    create_account("utilisateur.txt", u);

    // Provide feedback to the user
    g_print("Account creation process completed.\n");
    gtk_label_set_text(GTK_LABEL(feedback_label2),"Account creation process completed.");
}
/* Boîte de dialogue de confirmation
        GtkWidget *confirmation_dialog = gtk_message_dialog_new(
            GTK_WINDOW(user_data),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Are u sure u wont to delete parking ?");
        
        gint response = gtk_dialog_run(GTK_DIALOG(confirmation_dialog));
        gtk_widget_destroy(confirmation_dialog);

        if (response == GTK_RESPONSE_YES) {
            // Suppression de l'agent dans le fichier
            supprimer(agent);

            
            // Boîte de dialogue de succès
            GtkWidget *success_dialog = gtk_message_dialog_new(
                GTK_WINDOW(user_data),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Le parking deleted successfully.");
            
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        }*/


void
on_MGAdminbuttonConnect_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *entrylogin = lookup_widget(GTK_WIDGET(button), "MGAdminlogentry");
 GtkWidget *entrypass = lookup_widget(GTK_WIDGET(button), "MGAdminPassword");
 GtkWidget *feedbacklogin = lookup_widget(GTK_WIDGET(button), "feedbacklogin");
 
 const char *login = gtk_entry_get_text(GTK_ENTRY(entrylogin));
 const char *pass = gtk_entry_get_text(GTK_ENTRY(entrypass));
  if (log_in("utilisateur.txt",login ,pass)==1){
    GtkWidget *AdminwindowLogin, *AdminwindowMain;
    AdminwindowMain = create_AdminwindowMain();
    gtk_widget_show(AdminwindowMain);
    AdminwindowLogin=gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(AdminwindowLogin)){
      gtk_widget_hide(AdminwindowLogin);
    }
  }
  else if(log_in("utilisateur.txt",login ,pass)==0)
    gtk_label_set_text(GTK_LABEL(feedbacklogin),"password incorrect");
  else if(log_in("utilisateur.txt",login ,pass)==-1)
    gtk_label_set_text(GTK_LABEL(feedbacklogin),"Login not found");
  else if(log_in("utilisateur.txt",login ,pass)==-2)
    gtk_label_set_text(GTK_LABEL(feedbacklogin),"Erreur opening file");
  
}


void
on_MGAdminbuttonNewAcc_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *AdminwindowNewAcc, *AdminwindowLogin;
 AdminwindowNewAcc = create_AdminwindowNewAcc();
 gtk_widget_show(AdminwindowNewAcc);
 AdminwindowLogin=gtk_widget_get_toplevel(GTK_WIDGET(button));
 if (GTK_IS_WINDOW(AdminwindowLogin)){
  gtk_widget_hide(AdminwindowLogin);
  }
}


void
on_MGAdminBacknewaccbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *AdminwindowNewAcc, *AdminwindowLogin;
 AdminwindowLogin = create_AdminwindowLogin();
 gtk_widget_show(AdminwindowLogin);
 AdminwindowNewAcc=gtk_widget_get_toplevel(GTK_WIDGET(button));
 if (GTK_IS_WINDOW(AdminwindowNewAcc)){
  gtk_widget_hide(AdminwindowNewAcc);
  }
}


void
on_MGAdminLogoutbutton_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *AdminwindowMain, *AdminwindowLogin;
 AdminwindowLogin = create_AdminwindowLogin();
 gtk_widget_show(AdminwindowLogin);
 AdminwindowMain=gtk_widget_get_toplevel(GTK_WIDGET(button));
 if (GTK_IS_WINDOW(AdminwindowMain)){
  gtk_widget_hide(AdminwindowMain);
  }
}


void
on_MGAdminParkManagebutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *AdminwindowMain, *AdminwindowParkConfig;
 AdminwindowParkConfig = create_AdminwindowParkConfig();
 gtk_widget_show(AdminwindowParkConfig);
 AdminwindowMain=gtk_widget_get_toplevel(GTK_WIDGET(button));
 if (GTK_IS_WINDOW(AdminwindowMain)){
  gtk_widget_hide(AdminwindowMain);
  }
}


void
on_MGAdminBacktomainbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
 GtkWidget *AdminwindowMain, *AdminwindowParkConfig;
 AdminwindowMain = create_AdminwindowMain();
 gtk_widget_show(AdminwindowMain);
 AdminwindowParkConfig=gtk_widget_get_toplevel(GTK_WIDGET(button));
 if (GTK_IS_WINDOW(AdminwindowParkConfig)){
  gtk_widget_hide(AdminwindowParkConfig);
  }
}


void
on_MGadminrefreshPARK_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{

show_tree_view(GTK_WIDGET(button));
}


void
on_MGSearchPark_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *entrysearch = lookup_widget(GTK_WIDGET(button), "MGsearchParkingadmin");
GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button), "MGlabelfeedback");
const char *searchparkid = gtk_entry_get_text(GTK_ENTRY(entrysearch));
 if (search_parking_by_id("parkdata.txt", searchparkid)==1)
    show_tree_view_search(GTK_WIDGET(button));
 else if(strlen(searchparkid) == 0)
    gtk_label_set_text(GTK_LABEL(feedback_label),"Error: All fields must be filled.");
 else
    gtk_label_set_text(GTK_LABEL(feedback_label),"Park ID not found"); 
}


void
on_MGadminrefreshWorker_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
show_worker_tree_view(GTK_WIDGET(button));
}


void
on_MGSearchWorker_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *entrysearch = lookup_widget(GTK_WIDGET(button), "MGsearchWorker");
GtkWidget *feedback_label = lookup_widget(GTK_WIDGET(button), "feedback2");
const char *worker_id = gtk_entry_get_text(GTK_ENTRY(entrysearch));
 if (searchworker_by_id("workerdata.txt",worker_id)==1)
    show_workersearch_tree_view(GTK_WIDGET(button));
 else if(strlen(worker_id) == 0)
    gtk_label_set_text(GTK_LABEL(feedback_label),"Error: All fields must be filled.");
 else
    gtk_label_set_text(GTK_LABEL(feedback_label),"worker ID not found");
}
void
on_MGadminBackorigine_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *AdminwindowLogin , *welcome;
welcome = create_welcome ();
  gtk_widget_show (welcome);

    AdminwindowLogin = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Check if 'login' is a valid window
    if (GTK_IS_WINDOW(AdminwindowLogin)) {
        gtk_widget_hide(AdminwindowLogin); // Hide the login window
        g_print("login window is now hidden.\n");
    } else {
        g_warning("The login widget is not a valid window.");
    }
}

