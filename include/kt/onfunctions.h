#ifndef ONFUNCTIONS_H
#define ONFUNCTIONS_H

void kt_on_button_clicked(GtkButton *button, gpointer user_data);
void kt_on_file_chosen(GtkNativeDialog *native, gint response_id, gpointer user_data);
void kt_on_exit_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);

#endif