#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <kth/headers.h>

bool kt_is_valid_number(char *str);
bool kt_is_valid_commandline_input(int argc, char **argv, CmdData **cmd_data);
bool kt_is_fps_too_large(double fps, int total_frames);
bool kt_is_fps_negative(double fps);

#endif