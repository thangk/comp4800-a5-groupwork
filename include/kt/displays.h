#ifndef DISPLAYS_H
#define DISPLAYS_H

gboolean kt_display_frames(gpointer user_data);
void kt_display_frame_info_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data);
void kt_display_filename_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data);
void kt_display_cairo_text(cairo_t *cr, const char *text, const char *position, int width, int height);
void kt_display_exit_confirmation_dialog(GtkWindow *parent);
void kt_display_avframe_on_canvas(GtkDrawingArea *canvas, cairo_t *cr, int width, int height, gpointer user_data);

#endif