#include <gtk/gtk.h>
#include <kt/globals.h>
#include <kth/consts.h>
#include <kt/creators.h>
#include <kt/utils.h>
#include <kt/states.h>

AppData *app_data;
VideoData *video_data;
CmdData *cmd_data;
ButtonData **app_buttons;
ButtonData **media_buttons;

void kt_init_a_global(void **global, size_t size, GtkApplication *app)
{

    *global = malloc(size);
    if (*global == NULL)
    {
        kt_kaplog("Error: Failed to allocate memory\n");
        exit(1);
    }
}

void kt_init_globals(GtkApplication *app)
{
    // Initialize app_data

    app_data = malloc(sizeof(AppData));
    if (app_data == NULL)
    {
        kt_kaplog("Error: Failed to allocate memory for app_data\n");
        exit(1);
    }

    char APP_WINDOW_TITLE[100];
    sprintf(APP_WINDOW_TITLE, "%s - %s", APP_NAME, APP_TITLE);

    app_data->window = kt_create_main_window(app, APP_WINDOW_TITLE);
    app_data->canvas = kt_create_main_canvas();
    app_data->frame_info_box = kt_create_frame_info_box();
    app_data->filename_info_box = kt_create_filename_info_box();
    app_data->image_box = kt_create_image_box();

    // Initialize video_data

    video_data = malloc(sizeof(VideoData));
    if (video_data == NULL)
    {
        kt_kaplog("Error: Failed to allocate memory for video_data\n");
        exit(1);
    }

    video_data->pFormatCtx = NULL;
    video_data->pCodecCtx = NULL;
    video_data->videoStream = -1;
    video_data->total_frames = 0;
    video_data->current_frame_index = 0;
    video_data->fps = 0.0;
    video_data->filename = "";
    video_data->animation_timer_id = 0;

    if (!isCommandlineMode)
    {
        // Initialize cmd_data if isCommandMode is false, that means it's not initialize in main.c
        cmd_data = malloc(sizeof(CmdData));
        if (cmd_data == NULL)
        {
            kt_kaplog("Error: Failed to allocate memory for cmd_data\n");
            exit(1);
        }
        cmd_data->filename = "";
        cmd_data->fps = 0.0;
    }

    // app_buttons and media_buttons are initialized in kt_create_buttons_group()

    gtk_widget_queue_draw(app_data->canvas);
    gtk_widget_queue_draw(app_data->frame_info_box);
}