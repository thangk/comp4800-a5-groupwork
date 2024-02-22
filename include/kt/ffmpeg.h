#ifndef FFMPEG_H
#define FFMPEG_H

#include <kth/headers.h>

AVFormatContext *kt_open_video_file(char *filename);
AVCodecContext *kt_get_codec_context(AVFormatContext *pFormatCtx, int videoStream);
AVFrame *kt_decode_nth_frame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, int videoStream, int nth);
AVFrame **kt_decode_xtoy_frames(VideoData *video, int start_frame_index, int end_frame_index);
AVFrame **kt_decode_n_frames(VideoData *video, int n_frames);
AVFrame **kt_decode_all_frames(VideoData *video);
int kt_find_video_stream(AVFormatContext *pFormatCtx);
int kt_calculate_total_frames(VideoData *video);
double kt_calculate_fps(VideoData *video);

AVFrame *kt_convert_frame_to_rgb24(AVFrame *src_frame);
AVFrame *kt_convert_yuv420p_to_rgb24(AVFrame *frame);

#endif
