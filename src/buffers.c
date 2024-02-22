#include <kth/headers.h>

int current_buffer_index = 0;
int buffers_ready_count = 0;
int fetched_buffer_index = 0;
int fetching_buffer_index = 0;
int buffers_writes_count = 0;
int buffered_frames_count = 0;
int stoppage_buffer = -1;
FrameBuffer **buffers;

pthread_mutex_t buffer_index_mutex = PTHREAD_MUTEX_INITIALIZER;

void kt_init_buffers()
{
    buffers = (FrameBuffer **)malloc(BUFFERS_ARRAY_SIZE);
    if (buffers == NULL)
    {
        kt_kaplog("Failed to allocate memory for buffers");
        exit(1);
    }
    for (int i = 0; i < BUFFERS_COUNT; i++)
    {
        buffers[i] = (FrameBuffer *)malloc(BUFFER_SIZE);
        buffers[i]->buffer_size = MAX_FRAMES_PER_BUFFER;
        buffers[i]->frames = NULL;
        buffers[i]->start_frame_index = 0;
        buffers[i]->end_frame_index = 0;
        buffers[i]->total_frames_count = 0;
        buffers[i]->current_frame_index = 0;
        buffers[i]->index = i;
        buffers[i]->is_empty = true;
        buffers[i]->is_filled = false;
    }
}

void kt_set_next_buffer()
{
    // if index < BUFFERS_COUNT, incre else 0
    current_buffer_index = current_buffer_index < BUFFERS_COUNT ? current_buffer_index + 1 : 0;
}

void *kt_free_buffer(void *arg)
{
    FrameBuffer *buffer = (FrameBuffer *)arg;

    // skip if buffer is empty
    if (buffer->is_empty)
    {
        kt_kaplog("\tbuffer %d is already empty. Skipping freeing process.", buffer->index);
        return NULL;
    }

    if (buffer->frames != NULL)
    {
        for (int i = 0; i < buffer->total_frames_count; i++)
        {
            if (buffer->frames[i] != NULL)
            {
                av_frame_free(&buffer->frames[i]);
                buffer->frames[i] = NULL;
            }
        }
        free(buffer->frames);
        buffer->frames = NULL;

        kt_kaplog("\tbuffer[%d]->frames freed\n", buffer->index);
    }
    // reset buffer fields
    buffer->buffer_size = MAX_FRAMES_PER_BUFFER;
    buffer->total_frames_count = 0;
    buffer->current_frame_index = 0;
    buffer->start_frame_index = 0;
    buffer->end_frame_index = 0;
    buffer->is_empty = true;
    buffer->is_filled = false;

    // keep buffer->index as is
    return NULL;
}

void *kt_fill_buffer(void *arg)
{
    FrameBuffer *buffer = (FrameBuffer *)arg;

    bool is_first_buffer = (buffers_writes_count++ == 0) ? true : false;

    if (is_first_buffer)
    {
        kt_kaplog("\n----------------------------------------------------------------------\n");
        kt_kaplog("bw\t| buffer_name\t| index range\t| buffered\t| unbuffered\n");
        kt_kaplog("----------------------------------------------------------------------");
    }

    kt_free_buffer(buffer);

    int is_fps_too_large = kt_is_fps_too_large(video_data->fps, video_data->total_frames);

    if (video_data->fps < 0 || is_fps_too_large)
    {
        if (is_fps_too_large)
        {
            kt_kaplog("\n\nVery large FPS value. More than total_frames, filling all in first buffer.\n");
        }
        else
        {
            kt_kaplog("\n\nvideo_data->fps: %f < 0. Filling all frames in buffer.\n", video_data->fps);
        }
        video_data->fps = (video_data->fps < 0) ? video_data->fps * -1 : video_data->fps;
        kt_all_in_buffer(buffer);
        return NULL;
    }

    fetching_buffer_index = buffer->index;

    int start_index;
    int end_index;
    int buffer_size_needed;
    int unbuffered_frames;

    int prev_buffer_index = kt_get_prev_buffer_index(buffer->index);

    if (is_first_buffer)
    {
        unbuffered_frames = video_data->total_frames;
        start_index = 0;
        buffer_size_needed = (int)FIRST_5_FRAMES(video_data->fps);
        end_index = buffer_size_needed - 1;
    }
    else
    {
        unbuffered_frames = video_data->total_frames - buffered_frames_count;
        start_index = buffers[prev_buffer_index]->end_frame_index + 1;
        buffer_size_needed = unbuffered_frames < buffer->buffer_size ? unbuffered_frames : buffer->buffer_size;
    }

    if (unbuffered_frames <= MAX_FRAMES_PER_BUFFER && stoppage_buffer == -1)
    {
        stoppage_buffer = buffer->index;
        kt_kaplog(". Final buffer.");
    }

    buffer->frames = kt_decode_n_frames(video_data, buffer_size_needed);

    int num_of_frames_in_buffer = 0;
    while (buffer->frames[num_of_frames_in_buffer] != NULL)
    {
        num_of_frames_in_buffer++;
    }

    if (!is_first_buffer)
    {
        end_index = num_of_frames_in_buffer + start_index - 1;
    }

    buffer->current_frame_index = 0;
    buffer->start_frame_index = start_index;
    buffer->end_frame_index = end_index;
    buffer->total_frames_count = num_of_frames_in_buffer;
    buffer->buffer_size = buffer_size_needed;
    buffer->is_empty = false;
    buffer->is_filled = true;

    fetched_buffer_index = buffer->index;
    buffered_frames_count += buffer->total_frames_count;
    unbuffered_frames -= buffer->total_frames_count;

    kt_kaplog("\tbuffer %d has %d frames", buffer->index, buffer->total_frames_count);

    kt_kaplog("\n%-4d\t| buffer %-3d\t| %5d - %-5d\t| %8d\t| %10d", buffers_writes_count, buffer->index, buffer->start_frame_index, buffer->end_frame_index, buffer->total_frames_count, unbuffered_frames);

    return NULL;
}

void *kt_all_in_buffer(void *arg)
{
    FrameBuffer *buffer = (FrameBuffer *)arg;

    fetching_buffer_index = buffer->index;

    buffer->frames = kt_decode_all_frames(video_data);

    stoppage_buffer = buffer->index;

    int num_of_frames_in_buffer = 0;
    while (buffer->frames[num_of_frames_in_buffer] != NULL)
    {
        num_of_frames_in_buffer++;
    }

    int start_index = 0;
    int buffer_size_needed = video_data->total_frames;
    int end_index = num_of_frames_in_buffer - 1;
    int unbuffered_frames = video_data->total_frames - buffered_frames_count;

    buffer->current_frame_index = 0;
    buffer->start_frame_index = start_index;
    buffer->end_frame_index = end_index;
    buffer->total_frames_count = num_of_frames_in_buffer;
    buffer->buffer_size = buffer_size_needed;
    buffer->is_empty = false;
    buffer->is_filled = true;

    fetched_buffer_index = buffer->index;
    buffered_frames_count += buffer->total_frames_count;
    unbuffered_frames -= video_data->total_frames - buffered_frames_count;

    kt_kaplog("\n%-4d\t| buffer %-3d\t| %5d - %-5d\t| %8d\t| %10d", buffers_writes_count, buffer->index, buffer->start_frame_index, buffer->end_frame_index, buffer->total_frames_count, unbuffered_frames);

    return NULL;
}

bool kt_is_buffer_done()
{
    if (!buffers)
    {
        kt_kaplog("buffers is NULL\n");
    }
    if (!buffers[current_buffer_index])
    {
        kt_kaplog("buffers[%d] is NULL\n", current_buffer_index);
    }
    if (buffers[current_buffer_index]->current_frame_index < buffers[current_buffer_index]->total_frames_count)
    {
        return false;
    }

    return true;
}

int kt_get_next_buffer_index(int current_buffer)
{
    return (current_buffer + 1 == BUFFERS_COUNT) ? 0 : current_buffer + 1;
}

int kt_get_prev_buffer_index(int current_buffer)
{
    return (current_buffer - 1 < 0) ? BUFFERS_COUNT - 1 : current_buffer - 1;
}