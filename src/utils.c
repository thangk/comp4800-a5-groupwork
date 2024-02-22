#include <kth/headers.h>

void kt_kaplog(const gchar *format, ...)
{
    if (!isDebugMode)
    {
        return;
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

// TODO make proper cleanup of video_data and app_data, and the buffers
void kt_cleanup()
{
    kt_kaplog("Freeing some memory back to the system.\n");
}

void kt_append_to_box(GtkWidget *box, GtkWidget **widgets, int count)
{
    for (int i = 0; i < count; i++)
    {
        gtk_box_append(GTK_BOX(box), widgets[i]);
    }
}

int kt_len_ptr(void **array)
{
    int i = 0;
    while (array[i])
    {
        i++;
    }
    return i;
}