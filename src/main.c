#include <kth/headers.h>

static void kt_activate(GtkApplication *app, gpointer user_data)
{
    kt_init_globals(app);
    kt_init_buffers();

    const char *app_button_labels[] = {"Open", "Exit"};
    const char *media_control_labels[] = {"Play", "Reset"};

    GtkWidget *app_buttons_box = kt_create_buttons_group(app_button_labels, &app_buttons, kt_len(app_button_labels));
    GtkWidget *media_controls_box = kt_create_buttons_group(media_control_labels, &media_buttons, kt_len(media_control_labels));
    GtkWidget *main_box = kt_create_main_vertical_box(app_data->window);

    GtkWidget *widgets[] = {
        app_buttons_box,
        app_data->filename_info_box,
        app_data->canvas,
        app_data->frame_info_box,
        media_controls_box,
    };

    kt_append_to_box(main_box, widgets, kt_len(widgets));
    gtk_widget_show(app_data->window);

    if (isCommandlineMode)
    {
        kt_open_video(video_data, NULL, NULL);
        kt_first_video_load();
    }
}

// Custom command-line handler
static int kt_commandline(GApplication *app, GApplicationCommandLine *cmdline, gpointer user_data)
{

    // Retrieve arguments
    gchar **argv = g_application_command_line_get_arguments(cmdline, NULL);

    int status = kt_is_valid_commandline_input(kt_len_ptr((void **)argv), argv, &cmd_data);

    g_strfreev(argv);

    // if status is false, exit the program because of invalid command-line arguments
    if (!status)
    {
        g_print("\nInvalid argument values. Exiting...\n\n");
        return !status;
    }

    g_application_activate(app);
    return !status;
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new(APP_CODENAME, G_APPLICATION_HANDLES_COMMAND_LINE);

    g_signal_connect(app, "command-line", G_CALLBACK(kt_commandline), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(kt_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
