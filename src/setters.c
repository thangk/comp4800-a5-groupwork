#include <kth/headers.h>

void kt_set_color(GtkWidget *widget, char *widget_type, char *bgcolor, char *textcolor)
{
    // Check if the widget is valid
    if (!GTK_IS_WIDGET(widget))
    {
        g_warning("Invalid widget passed to kt_set_color");
        return;
    }

    // Prepare the CSS string
    char css[100];
    snprintf(css, sizeof(css), "%s { background: %s; color: %s; }", widget_type, bgcolor, textcolor);

    // Create a new CSS provider
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_data(provider, css, -1);

    // Check for errors
    if (error)
    {
        g_warning("Failed to load CSS: %s", error->message);
        g_error_free(error);
        return;
    }

    gtk_style_context_add_provider(gtk_widget_get_style_context(widget), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void kt_set_font(GtkWidget *widget, char *widget_type, char *fontWeight, int fontSize)
{
    if (!GTK_IS_WIDGET(widget))
    {
        g_warning("Invalid widget passed to kt_set_font");
        return;
    }

    char css[100];
    snprintf(css, sizeof(css), "%s { font-weight: %s; font-size: %dpx; }", widget_type, fontWeight, fontSize);

    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_data(provider, css, -1);
    if (error)
    {
        g_warning("Failed to load CSS: %s", error->message);
        g_error_free(error);
        return;
    }

    gtk_style_context_add_provider(gtk_widget_get_style_context(widget), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
