#ifndef SETTERS_H
#define SETTERS_H

#include <kth/headers.h>

void kt_set_color(GtkWidget *widget, char *widget_type, char *bgcolor, char *textcolor);
void kt_set_font(GtkWidget *widget, char *widget_type, char *fontWeight, int fontSize);
bool kt_set_commandline_mode(int argc, char **argv);

#endif
