#ifndef UTILS_H
#define UTILS_H

#define kt_len(array) (sizeof(array) / sizeof((array)[0]))

#include <kth/headers.h>

void kt_cleanup();
void kt_kaplog(const gchar *format, ...);
void kt_append_to_box(GtkWidget *box, GtkWidget **widgets, int count);
int kt_len_ptr(void **array);

#endif