#include <gtk/gtk.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <kt/globals.h>
#include <kt/states.h>
#include <kt/ffmpeg.h>
#include <kt/utils.h>
#include <stdlib.h>
#include <kt/buffers.h>

double kt_calculate_fps(VideoData *video)
{
    AVStream *stream = video->pFormatCtx->streams[video->videoStream];
    double fps = av_q2d(stream->avg_frame_rate);
    if (fps < 0.01)
    {
        fps = 1.0 / av_q2d(stream->time_base);
    }
    return fps;
}

int kt_calculate_total_frames(VideoData *video)
{
    AVStream *stream = video->pFormatCtx->streams[video->videoStream];
    int64_t duration = stream->duration;         // Duration of the stream in AV_TIME_BASE units
    AVRational time_base = stream->time_base;    // Time base of the stream
    double fps = av_q2d(stream->avg_frame_rate); // Frames per second

    if (fps < 0.01)
    { // Check for valid frame rate
        // Fallback to calculating fps using time_base if avg_frame_rate is not available
        fps = 1.0 / av_q2d(stream->time_base);
    }

    // Calculate total frames by multiplying duration by fps
    long totalFrames = (long)(duration * fps * av_q2d(time_base));
    return totalFrames;
}

// 1. Open the file
AVFormatContext *kt_open_video_file(char *filename)
{
    AVFormatContext *pFormatCtx = NULL;
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
    {
        fprintf(stderr, "Could not open video file: %s\n", filename);
        return NULL;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        avformat_close_input(&pFormatCtx);
        return NULL;
    }
    return pFormatCtx;
}

// 2. Find the video stream
int kt_find_video_stream(AVFormatContext *pFormatCtx)
{
    int videoStream = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    return videoStream;
}

// 3. Find the decoder
AVCodecContext *kt_get_codec_context(AVFormatContext *pFormatCtx, int videoStream)
{
    AVCodecParameters *codecParams = pFormatCtx->streams[videoStream]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(codecParams->codec_id);
    if (pCodec == NULL)
    {
        fprintf(stderr, "Unsupported codec!\n");
        return NULL;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, codecParams) < 0)
    {
        fprintf(stderr, "Failed to copy codec parameters to codec context\n");
        return NULL;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        fprintf(stderr, "Failed to open codec\n");
        return NULL;
    }

    return pCodecCtx;
}

AVFrame *kt_convert_frame_to_rgb24(AVFrame *src_frame)
{
    if (!src_frame)
    {
        return NULL;
    }

    // Initialize the software scaling context.
    struct SwsContext *sws_ctx = sws_getContext(
        src_frame->width, src_frame->height, src_frame->format,
        src_frame->width, src_frame->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);

    if (!sws_ctx)
    {
        kt_kaplog("Failed to initialize sws context\n");
        return NULL;
    }

    // Allocate memory for the destination frame.
    AVFrame *dst_frame = av_frame_alloc();
    if (!dst_frame)
    {
        kt_kaplog("Failed to allocate cairo_frame\n");
        sws_freeContext(sws_ctx);
        return NULL;
    }

    dst_frame->format = AV_PIX_FMT_RGB24;
    dst_frame->width = src_frame->width;
    dst_frame->height = src_frame->height;

    // Allocate memory for the frame's data.
    int ret = av_image_alloc(dst_frame->data, dst_frame->linesize, dst_frame->width,
                             dst_frame->height, AV_PIX_FMT_RGB24, 32);
    if (ret < 0)
    {
        kt_kaplog("Could not allocate the video frame data\n");
        av_frame_free(&dst_frame);
        sws_freeContext(sws_ctx);
        return NULL;
    }

    // Perform the conversion.
    sws_scale(sws_ctx,
              (const uint8_t *const *)src_frame->data, src_frame->linesize,
              0, src_frame->height,
              dst_frame->data, dst_frame->linesize);

    // Free the scaling context.
    sws_freeContext(sws_ctx);

    // Adjust the stride to match Cairo's requirements.
    int cairo_stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, dst_frame->width);
    uint8_t *new_buffer = (uint8_t *)malloc(dst_frame->height * cairo_stride);
    if (!new_buffer)
    {
        kt_kaplog("Failed to allocate new_buffer\n");
        av_frame_free(&dst_frame);
        dst_frame = NULL;
        return NULL;
    }

    // Copy data from dst_frame to new_buffer, row by row.
    for (int y = 0; y < dst_frame->height; y++)
    {
        memcpy(new_buffer + y * cairo_stride, dst_frame->data[0] + y * dst_frame->linesize[0], dst_frame->width * 3);
    }

    // Update dst_frame to use the new buffer.
    av_freep(&dst_frame->data[0]);
    dst_frame->data[0] = new_buffer;
    dst_frame->linesize[0] = cairo_stride;

    return dst_frame;
}

AVFrame *kt_convert_yuv420p_to_rgb24(AVFrame *frame)
{
    // Allocate memory for the output buffer
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
    unsigned char *buffer = (unsigned char *)av_malloc(num_bytes * sizeof(unsigned char));

    // Create a context for the conversion
    struct SwsContext *sws_ctx = sws_getContext(
        frame->width, frame->height, frame->format,    // Source format and size
        frame->width, frame->height, AV_PIX_FMT_RGB24, // Target format and size
        SWS_BILINEAR,                                  // Scaling algorithm
        NULL, NULL, NULL);

    if (!sws_ctx)
    {
        fprintf(stderr, "Could not create sws context\n");
        av_free(buffer);
        return NULL;
    }

    // Set up the AVFrame for the destination image
    AVFrame *rgb_frame = av_frame_alloc();
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24, frame->width, frame->height, 1);

    // Perform the conversion
    sws_scale(sws_ctx, (const uint8_t *const *)frame->data, frame->linesize, 0, frame->height, rgb_frame->data, rgb_frame->linesize);

    // Clean up
    sws_freeContext(sws_ctx);

    return rgb_frame;
}

// 4.2 Decode frame
AVFrame **kt_decode_xtoy_frames(VideoData *video, int start_frame_index, int end_frame_index)
{

    // in 25 elements, 0-24, 0-24
    int total_frames = end_frame_index - start_frame_index + 1;

    if (video->pFormatCtx == NULL)
    {
        return NULL;
    }

    // use calloc to allocate memory and initialize all elements to NULL
    AVFrame **frames = calloc(total_frames, sizeof(AVFrame *));

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        free(frames);
        return NULL;
    }

    AVPacket packet;

    int decode_frame_index = 0;
    int frame_array_index = 0;

    kt_kaplog("\t\tDecoding %d frames.\n", total_frames);

    while (av_read_frame(video->pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == video->videoStream)
        {
            avcodec_send_packet(video->pCodecCtx, &packet);
            while (avcodec_receive_frame(video->pCodecCtx, pFrame) == 0)
            {
                if (decode_frame_index >= start_frame_index && decode_frame_index <= end_frame_index)
                {
                    AVFrame *converted_frame = kt_convert_frame_to_rgb24(pFrame);
                    if (converted_frame && frame_array_index < total_frames)
                    {
                        frames[frame_array_index++] = converted_frame;
                    }
                    else
                    {
                        kt_kaplog("Conversion to RGB24 failed. Frame was not stored to frame array.\n");
                    }
                }

                if (decode_frame_index > end_frame_index)
                {
                    kt_kaplog("\tdecode_frame_index: %d, frame_array_index: %d, end_frame_index: %d\n", decode_frame_index, frame_array_index, end_frame_index);
                    return frames;
                }
                decode_frame_index++;
                // kt_kaplog("\tFrames dectected counter: %d\n", decode_frame_index);
            }
            av_packet_unref(&packet);
        }
    }

    av_frame_free(&pFrame);
    pFrame = NULL;

    if (decode_frame_index <= end_frame_index)
    {
        kt_kaplog("\tdecode_frame_index: %d, frame_array_index: %d, end_frame_index: %d\n", decode_frame_index, frame_array_index, end_frame_index);

        kt_kaplog("\n\nERROR: something went wrong, frames were not stored into frame array.\n\n");
    }
    return frames;
}

AVFrame **kt_decode_n_frames(VideoData *video, int n_frames)
{

    if (video->pFormatCtx == NULL)
    {
        return NULL;
    }

    // use calloc to allocate memory and initialize all elements to NULL
    AVFrame **frames = calloc(n_frames + 1, sizeof(AVFrame *));

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        free(frames);
        return NULL;
    }

    AVPacket packet;

    int decode_frame_index = 0;
    int frame_array_index = 0;

    while (av_read_frame(video->pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == video->videoStream)
        {
            avcodec_send_packet(video->pCodecCtx, &packet);
            while (avcodec_receive_frame(video->pCodecCtx, pFrame) == 0)
            {
                if (decode_frame_index >= n_frames)
                    return frames;

                if (decode_frame_index < n_frames)
                {

                    // frames[frame_array_index++] = pFrame;
                    AVFrame *converted_frame = kt_convert_frame_to_rgb24(pFrame);
                    if (converted_frame)
                    {
                        frames[frame_array_index++] = converted_frame;
                    }
                    else
                    {
                        kt_kaplog("Conversion to RGB24 failed. Frame was not stored to frame array.\n");
                    }
                }
                decode_frame_index++;
            }
            av_packet_unref(&packet);
        }
    }

    av_frame_free(&pFrame);
    pFrame = NULL;

    if (decode_frame_index <= n_frames)
    {
        // kt_kaplog("\tdecode_frame_index: %d, frame_array_index: %d\n", decode_frame_index, frame_array_index);

        // kt_kaplog("\n\nERROR: something went wrong, frames were not stored into frame array.\n\n");
    }
    return frames;
}

AVFrame **kt_decode_all_frames(VideoData *video)
{
    if (video->pFormatCtx == NULL)
    {
        return NULL;
    }

    AVFrame **frames = calloc(video_data->total_frames, sizeof(AVFrame *));
    if (!frames)
    {
        return NULL;
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        free(frames);
        return NULL;
    }

    AVPacket packet;
    int frame_array_index = 0;

    while (av_read_frame(video->pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == video->videoStream)
        {
            avcodec_send_packet(video->pCodecCtx, &packet);
            while (avcodec_receive_frame(video->pCodecCtx, pFrame) == 0)
            {
                AVFrame *converted_frame = kt_convert_frame_to_rgb24(pFrame);
                if (converted_frame)
                {
                    frames[frame_array_index++] = converted_frame;
                }
                else
                {
                    kt_kaplog("Conversion to RGB24 failed. Frame was not stored to frame array.\n");
                }
            }
            av_packet_unref(&packet);
        }
    }

    av_frame_free(&pFrame);
    pFrame = NULL;
    return frames;
}

// 4. Decode nth frame
AVFrame *kt_decode_nth_frame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx, int videoStream, int nth)
{
    AVFrame *pFrame = av_frame_alloc();
    AVPacket packet;
    int frameCount = 0;

    while (av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == videoStream)
        {
            avcodec_send_packet(pCodecCtx, &packet);
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0)
            {
                // Increment frame count when a frame is successfully decoded
                frameCount++;
                if (frameCount == nth)
                {
                    av_packet_unref(&packet);
                    return pFrame; // Return the nth frame
                }
                // Free the frame if it's not the one we're looking for, to avoid memory leaks
                av_frame_unref(pFrame);
            }
        }
        av_packet_unref(&packet);
    }

    // If we reach this point, it means the nth frame was not found
    av_frame_free(&pFrame);
    pFrame = NULL;
    return NULL;
}
