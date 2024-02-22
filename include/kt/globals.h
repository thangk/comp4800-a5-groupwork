#ifndef GLOBALS_H
#define GLOBALS_H

#include <kth/headers.h>

extern AppData *app_data;
extern VideoData *video_data;
extern CmdData *cmd_data;
extern ButtonData **app_buttons;
extern ButtonData **media_buttons;

// mutexes
extern pthread_t prefetch_thread;
extern pthread_mutex_t prefetch_mutex;

#endif