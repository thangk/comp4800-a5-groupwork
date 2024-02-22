#include <kth/headers.h>

// TODO maybe it's good enough?
gboolean kt_display_frames(gpointer user_data)
{

    if (isPlaying)
    {

        int prev = kt_get_prev_buffer_index(current_buffer_index);
        int next = kt_get_next_buffer_index(current_buffer_index);
        int next_next = kt_get_next_buffer_index(next);
        int current = current_buffer_index;

        if (kt_is_buffer_done() && cmd_data->fps >= 0)
        {

            kt_kaplog("\tbuffer %d is done, reading from %d, and filling %d\t", current, next, next_next);
            kt_set_next_buffer();

            pthread_t thread;
            pthread_create(&thread, NULL, kt_fill_buffer, buffers[next_next]);
            pthread_detach(thread);

            return isPlaying;
        }

        int is_last_frame = 0;
        int in_last_buffer = (current_buffer_index == stoppage_buffer) ? 1 : 0;

        if (in_last_buffer)
        {
            is_last_frame = buffers[current_buffer_index]->current_frame_index + 1 == buffers[stoppage_buffer]->total_frames_count - 1 ? 1 : 0;
        }

        if (stoppage_buffer >= 0)
        {
            // kt_kaplog("video_data %d\tbuffers[%d]-%d\tstoppage [%d]-%d\n", video_data->current_frame_index, current_buffer_index, buffers[current_buffer_index]->current_frame_index, stoppage_buffer, buffers[stoppage_buffer]->total_frames_count);
        }

        if (is_last_frame)
        {
            kt_kaplog("\t\tVideo finished playing.\n", NULL);
            kt_pause_video();
            isPlaying = false;
            return isPlaying;
        }

        gtk_widget_queue_draw(app_data->canvas);
        gtk_widget_queue_draw(app_data->frame_info_box);

        video_data->current_frame_index++;
        buffers[current_buffer_index]->current_frame_index++;
    }
    return isPlaying ? true : false;
}

void kt_display_avframe_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data)
{
    if (video_data->pFormatCtx == NULL)
    {
        return;
    }

    AVFrame *cairo_frame = buffers[current_buffer_index]
                               ->frames[buffers[current_buffer_index]->current_frame_index];

    if (cairo_frame != NULL && cairo_frame->data[0] != NULL)
    {
        cairo_surface_t *surface = cairo_image_surface_create_for_data(
            cairo_frame->data[0],
            CAIRO_FORMAT_RGB24,
            cairo_frame->width,
            cairo_frame->height,
            cairo_frame->linesize[0]);

        // Check if the surface creation was successful
        if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS)
        {
            cairo_save(cr); // Save the current drawing context state
            cairo_set_source_surface(cr, surface, 0, 0);
            cairo_paint(cr);
            cairo_restore(cr);              // Restore the drawing context state
            cairo_surface_destroy(surface); // Clean up the Cairo surface
        }

        // av_frame_free(&cairo_frame);
        cairo_frame = NULL;
    }
}

void kt_display_frame_info_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data)
{
    char *frame_info_string = g_strdup_printf("Frame: %d / %d", video_data->current_frame_index, video_data->total_frames);
    char *fps_info_string = g_strdup_printf("FPS: %.3f", video_data->fps);
    kt_display_cairo_text(cr, frame_info_string, "left", width, height);
    kt_display_cairo_text(cr, fps_info_string, "right", width, height);
}

void kt_display_filename_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data)
{
    char *filename_info_string = g_strdup_printf("File: %s", video_data->filename);
    kt_display_cairo_text(cr, filename_info_string, "left", width, height);
}

void kt_display_cairo_text(cairo_t *cr, const char *text, const char *position, int width, int height)
{
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    double x;
    double y;

    if (strcmp(position, "left") == 0)
    {
        x = 0;
        y = (height / 2);
    }

    if (strcmp(position, "center") == 0)
    {
        x = width / 2 - extents.width / 2;
        y = height / 2;
    }

    if (strcmp(position, "right") == 0)
    {
        x = width - extents.width - 10;
        y = (height / 2);
    }

    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, CAIRO_FONT_SIZE);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
}

void kt_display_exit_confirmation_dialog(GtkWindow *parent)
{
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_YES_NO,
                                               "Are you sure you want to exit?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Exit");

    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(kt_on_exit_dialog_response), parent);

    gtk_widget_show(dialog);
}
