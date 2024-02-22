#ifndef CONSTS_H
#define CONSTS_H

// app info
#define APP_NAME "COMP4800.A5"
#define APP_TITLE "Multithreaded Video Player"
#define APP_CODENAME "com.example.multithreadedframeextractor"

// struct data sizes
#define APP_DATA_SIZE sizeof(AppData)
#define VIDEO_DATA_SIZE sizeof(VideoData)
#define BUTTON_DATA_SIZE sizeof(ButtonData)

// buffer consts
#define FIRST_5_FRAMES(fps) (fps * 5)
#define MAX_FRAMES_PER_BUFFER 1000
#define BUFFER_SIZE sizeof(FrameBuffer)
#define BUFFERS_COUNT 3
#define BUFFERS_ARRAY_SIZE (sizeof(FrameBuffer) * BUFFERS_COUNT)

// default init values
#define CAIRO_FONT_FAMILY "Sans"
#define CAIRO_FONT_SIZE 24
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024
#define CANVAS_WIDTH 960
#define CANVAS_HEIGHT 540

#endif