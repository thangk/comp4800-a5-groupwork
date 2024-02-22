#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <kth/headers.h>

typedef struct
{
    GtkWidget *window;
    GtkWidget *canvas;
    GtkWidget *frame_info_box;
    GtkWidget *filename_info_box;
    GtkWidget *image_box;
} AppData;

typedef struct
{
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    int videoStream;
    int total_frames;
    int current_frame_index;
    double fps;
    char *filename;
    int animation_timer_id;
} VideoData;

typedef struct
{
    char *filename;
    double fps;
} CmdData;

typedef struct
{
    GtkWidget *button;
    const char *label;
    int index;
} ButtonData;

typedef struct
{
    int start_frame_index;
    int end_frame_index;
    int current_frame_index;
    int buffer_size; // max frames per buffer
    bool is_filled;

    int total_frames_count; // total actual frames in buffer
    int index;
    bool is_empty;
    AVFrame **frames;
} FrameBuffer;

#endif