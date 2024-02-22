#ifndef MEDIA_CONTROLS_H
#define MEDIA_CONTROLS_H

void kt_open_video(VideoData *video, GFile *file, GtkNativeDialog *native);
void kt_play_video();
void kt_pause_video();
void kt_reset_video();
void kt_first_video_load();

#endif