#include <kth/headers.h>

GtkWidget *kt_create_buttons_group(const char *button_labels[], ButtonData ***buttons, int num_labels)
{
    // initialize the button group
    *buttons = (ButtonData **)malloc(num_labels * sizeof(ButtonData *));
    if (*buttons == NULL)
    {
        kt_kaplog("Error: Failed to allocate memory for buttons");
        return NULL;
    }

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
    for (int i = 0; i < num_labels; i++)
    {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        kt_set_color(button, "button", "lightgrey", "black");
        g_signal_connect(button, "clicked", G_CALLBACK(kt_on_button_clicked), app_data);

        // store the button data
        (*buttons)[i] = (ButtonData *)malloc(sizeof(ButtonData));
        if ((*buttons)[i] == NULL)
        {
            kt_kaplog("Error: Failed to allocate memory for buttons[i]");
            return NULL;
        }
        (*buttons)[i]->button = button;
        (*buttons)[i]->label = strdup(button_labels[i]);
        (*buttons)[i]->index = i;

        gtk_box_append(GTK_BOX(button_box), (*buttons)[i]->button);
    }
    return button_box;
}
GtkWidget *kt_create_frame_counter()
{
    GtkWidget *frame_number_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(frame_number_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(frame_number_box, 25);
    gtk_widget_set_margin_bottom(frame_number_box, 25);
    kt_set_font(frame_number_box, "box", "bold", 20);

    char build_string[50];
    sprintf(build_string, "Frame Number: %d / %d", video_data->current_frame_index, video_data->current_frame_index);

    gtk_box_append(GTK_BOX(frame_number_box), gtk_label_new(build_string));
    return frame_number_box;
}
GtkWidget *kt_create_main_canvas()
{
    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(canvas), CANVAS_WIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(canvas), CANVAS_HEIGHT);
    gtk_widget_set_halign(canvas, GTK_ALIGN_CENTER);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(canvas), kt_display_avframe_on_canvas, NULL, NULL);
    kt_set_color(canvas, "widget", "darkgrey", "black");
    return canvas;
}
GtkWidget *kt_create_frame_info_box()
{
    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(canvas), CANVAS_WIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(canvas), 100);
    gtk_widget_set_halign(canvas, GTK_ALIGN_CENTER);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(canvas), kt_display_frame_info_on_canvas, NULL, NULL);
    return canvas;
}
GtkWidget *kt_create_filename_info_box()
{
    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(canvas), CANVAS_WIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(canvas), 100);
    gtk_widget_set_halign(canvas, GTK_ALIGN_CENTER);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(canvas), kt_display_filename_on_canvas, NULL, NULL);
    return canvas;
}
GtkWidget *kt_create_main_vertical_box(GtkWidget *parent_window)
{
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(vbox, 25);
    gtk_widget_set_margin_bottom(vbox, 25);
    gtk_window_set_child(GTK_WINDOW(parent_window), vbox);
    return vbox;
}
GtkWidget *kt_create_image_box()
{
    GtkWidget *image_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_halign(image_box, GTK_ALIGN_CENTER);
    gtk_box_set_spacing(GTK_BOX(image_box), 10);
    return image_box;
}
GtkWidget *kt_create_main_window(GtkApplication *app, const char *title)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, 0);
    return window;
}
