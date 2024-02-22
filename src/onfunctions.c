#include <kth/headers.h>


void kt_on_button_clicked(GtkButton *button, gpointer user_data)
{
    const char *button_label = gtk_button_get_label(button);

    if (strcmp(button_label, "Open") == 0)
    {
        GtkFileChooserNative *native = gtk_file_chooser_native_new("Open Video",
                                                                   GTK_WINDOW(app_data->window),
                                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                   "_Open",
                                                                   "_Cancel");
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(native), TRUE);

        g_signal_connect(native, "response", G_CALLBACK(kt_on_file_chosen), app_data->canvas);
        gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
    }

    if (strcmp(button_label, "Exit") == 0)
    {
        kt_display_exit_confirmation_dialog(GTK_WINDOW(app_data->window));
    }

    if (strcmp(button_label, "Play") == 0)
    {
        isPlaying = !isPlaying;

        // do nothing if the video is not loaded
        if (video_data->pFormatCtx == NULL)
        {
            kt_kaplog("Error: video not loaded\n");
            return;
        }

        if (isPlaying)
            kt_play_video();

        if (!isPlaying)
            kt_pause_video();
    }

    // reset the video to the first frame, reset other labels and counters
    if (strcmp(button_label, "Reset") == 0)
    {
        // TODO redo this
        // reset the video to the first frame and initiate first_video_load
        kt_reset_video();
        kt_first_video_load();
    }
}

void kt_on_file_chosen(GtkNativeDialog *native, gint response_id, gpointer user_data)
{
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        isCommandlineMode = false;
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(native));

        kt_open_video(video_data, file, native);
        kt_first_video_load();
    }
}

void kt_on_exit_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    if (response_id == GTK_RESPONSE_YES)
    {
        g_application_quit(G_APPLICATION(gtk_window_get_application(GTK_WINDOW(user_data))));
        g_signal_connect(dialog, "destroy", G_CALLBACK(kt_cleanup), NULL);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}
