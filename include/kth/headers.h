#ifndef HEADERS_H
#define HEADERS_H

#include <gtk/gtk.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include <kth/typedefs.h>
#include <kth/consts.h>
#include <kt/buffers.h>
#include <kt/creators.h>
#include <kt/displays.h>
#include <kt/ffmpeg.h>
#include <kt/setters.h>
#include <kt/globals.h>
#include <kt/init_globals.h>
#include <kt/media_controls.h>
#include <kt/onfunctions.h>
#include <kt/states.h>
#include <kt/utils.h>
#include <kt/validators.h>

#endif