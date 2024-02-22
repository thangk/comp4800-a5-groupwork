#include <kth/headers.h>

void kt_open_video(VideoData *video, GFile *file, GtkNativeDialog *native)
{
    // this function assigns the following fields
    // [1] AVFormatContext *pFormatCtx
    // [2] int videoStream
    // [3] AVCodecContext *pCodecCtx
    // [4] double fps
    // [5] int total_frames
    // [6] int current_frame_index
    // [7] char *filename

    // deocoding the actual frames will be stored in the buffers, as that's the most CPU instensive task and needs to be dynamic on how many frames to decode at a time (ie. we don't need to decode the whole video on first load, maybe just 5 frames and keep it paused then have 2nd buffer decode another 100 frames in bg)    Kap

    kt_kaplog("\n\nLoading video...");

    GFile *cmd_file = g_file_new_for_path(cmd_data->filename);

    char *filepath = isCommandlineMode ? g_file_get_path(cmd_file) : g_file_get_path(file);

    // [1] Open the video file
    video->pFormatCtx = kt_open_video_file(filepath);

    if (!video->pFormatCtx)
        g_free(filepath);

    // [2] Find the video stream
    video->videoStream = kt_find_video_stream(video->pFormatCtx);

    if (video->videoStream == -1)
        g_free(filepath);

    // [3] Find the decoder
    video->pCodecCtx = kt_get_codec_context(video->pFormatCtx, video->videoStream);

    if (!video->pCodecCtx)
    {
        avformat_close_input(&video->pFormatCtx);
        g_free(filepath);
    }

    // [4], [5] Set FPS and total frames
    if (video->videoStream >= 0)
    {
        video_data->total_frames = kt_calculate_total_frames(video);
        video_data->fps = isCommandlineMode ? cmd_data->fps : kt_calculate_fps(video);
    }

    // [6], [7]
    video->current_frame_index = 0;
    video_data->filename = isCommandlineMode ? g_file_get_basename(cmd_file) : g_file_get_basename(file);

    kt_kaplog("\tLoaded.");
}

void kt_play_video()
{
    // only play if fps is established
    if (video_data->animation_timer_id == 0 && video_data->fps > 0)
    {
        const int frame_speed = 1000 / video_data->fps;
        video_data->animation_timer_id = g_timeout_add(frame_speed, kt_display_frames, NULL);

        for (int i = 0; i < 2; i++)
        {
            if (strcmp(media_buttons[i]->label, "Play") == 0)
                kt_set_color(media_buttons[i]->button, "button", "black", "white");
        }
    }
}

void kt_pause_video()
{
    // can only pause if it's being played currently
    if (video_data->animation_timer_id != 0)
    {
        g_source_remove(video_data->animation_timer_id);
        video_data->animation_timer_id = 0;

        for (int i = 0; i < 2; i++)
        {
            if (strcmp(media_buttons[i]->label, "Play") == 0)
                kt_set_color(media_buttons[i]->button, "button", "lightgrey", "black");
        }
    }
}

void kt_reset_video()
{
    // reset the video to the first frame
    kt_pause_video();
    video_data->current_frame_index = 0;
}

void kt_first_video_load()
{

    kt_kaplog("Inside first_video_load\n");

    kt_fill_buffer(buffers[current_buffer_index]);

    // update UI immediately after loading the first buffer
    gtk_widget_queue_draw(app_data->canvas);
    gtk_widget_queue_draw(app_data->filename_info_box);
    gtk_widget_queue_draw(app_data->frame_info_box);

    bool is_fps_too_large = kt_is_fps_too_large(video_data->fps, video_data->total_frames);

    if (cmd_data->fps >= 0 && !is_fps_too_large)
    {
        pthread_t thread2;
        pthread_create(&thread2, NULL, kt_fill_buffer, buffers[kt_get_next_buffer_index(current_buffer_index)]);
    }
}