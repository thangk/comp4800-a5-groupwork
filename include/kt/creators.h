#ifndef CREATORS_H
#define CREATORS_H

GtkWidget *kt_create_main_window(GtkApplication *app, const char *title);
GtkWidget *kt_create_image_box();
GtkWidget *kt_create_main_vertical_box(GtkWidget *parent_window);
GtkWidget *kt_create_filename_info_box();
GtkWidget *kt_create_frame_info_box();
GtkWidget *kt_create_main_canvas();
GtkWidget *kt_create_frame_counter();
GtkWidget *kt_create_buttons_group(const char *button_labels[], ButtonData ***buttons, int num_labels);

#endif