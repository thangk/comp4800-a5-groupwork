#include <kth/headers.h>

bool isCommandlineMode = false; //  if true, loads video right away as soon as video shows up from commandline input
bool isUnbufferedMode = false;  //  if false, all frames will load to 1 buffer
bool isDebugMode = true;        //  if false, disables kt_kaplogs
bool isPlaying = false;         //  if true, indicates video is being played
