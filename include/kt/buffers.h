#ifndef BUFFERS_H
#define BUFFERS_H

#include <kth/headers.h>

extern int current_buffer_index;
extern int fetched_buffer_index;
extern int fetching_buffer_index;
extern int buffers_writes_count;
extern int buffered_frames_count;
extern int stoppage_buffer;
extern FrameBuffer **buffers;
extern pthread_mutex_t buffer_index_mutex;

bool kt_is_buffer_done();
void kt_init_buffers();
void kt_set_next_buffer();
void *kt_free_buffer(void *arg);
void *kt_fill_buffer(void *arg);
void *kt_all_in_buffer(void *arg);
int kt_get_next_buffer_index(int current_buffer);
int kt_get_prev_buffer_index(int current_buffer);

#endif