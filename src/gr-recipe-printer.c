/* gr-recipe-printer.c
 *
 * Copyright (C) 2016 Matthias Clasen <mclasen@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gr-recipe-printer.h"

struct _GrRecipePrinter
{
        GObject parent_instance;

        GtkWindow *window;
        GtkPrintOperation *print;
        PangoLayout *layout;

        GrRecipe *recipe;
};

G_DEFINE_TYPE (GrRecipePrinter, gr_recipe_printer, G_TYPE_OBJECT)

enum {
        PROP_0,
        N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
gr_recipe_printer_finalize (GObject *object)
{
        GrRecipePrinter *self = (GrRecipePrinter *)object;

        G_OBJECT_CLASS (gr_recipe_printer_parent_class)->finalize (object);
}

static void
gr_recipe_printer_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
        GrRecipePrinter *self = GR_RECIPE_PRINTER (object);

        switch (prop_id)
          {
          default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
          }
}

static void
gr_recipe_printer_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
        GrRecipePrinter *self = GR_RECIPE_PRINTER (object);

        switch (prop_id)
          {
          default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
          }
}

static void
gr_recipe_printer_class_init (GrRecipePrinterClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->finalize = gr_recipe_printer_finalize;
        object_class->get_property = gr_recipe_printer_get_property;
        object_class->set_property = gr_recipe_printer_set_property;
}

static void
gr_recipe_printer_init (GrRecipePrinter *self)
{
}

GrRecipePrinter *
gr_recipe_printer_new (GtkWindow *parent)
{
        GrRecipePrinter *printer;

        printer = g_object_new (GR_TYPE_RECIPE_PRINTER, NULL);

        printer->window = parent;
}

static void
begin_print (GtkPrintOperation *operation,
             GtkPrintContext   *context,
             GrRecipePrinter   *printer)
{
        int width, height;
        PangoFontDescription *title_font;
        PangoFontDescription *body_font;
        PangoAttrList *attrs;
        PangoAttribute *attr;
        g_autoptr(GString) s = NULL;

        g_print ("begin_print\n");
        width = gtk_print_context_get_width (context);
        height = gtk_print_context_get_height (context);

        printer->layout = gtk_print_context_create_pango_layout (context);

        title_font = pango_font_description_from_string ("Cantarell Bold 24");
        body_font = pango_font_description_from_string ("Cantarell 18");
        pango_layout_set_font_description (printer->layout, body_font);

        attrs = pango_attr_list_new ();

        pango_layout_set_width (printer->layout, width * PANGO_SCALE);

        s = g_string_new ("");
        g_string_append_printf (s, "%s by %s",
                                gr_recipe_get_name (printer->recipe),
                                gr_recipe_get_author (printer->recipe));

        attr = pango_attr_font_desc_new (title_font);
        attr->start_index = 0;
        attr->end_index = s->len + 1;
        pango_attr_list_insert (attrs, attr);

        g_string_append_printf (s, "\n\n%s\n\n", gr_recipe_get_description (printer->recipe));

        attr = pango_attr_font_desc_new (title_font);
        attr->start_index = s->len;

        g_string_append (s, "Ingredients");

        attr->end_index = s->len + 1;
        pango_attr_list_insert (attrs, attr);

        g_string_append (s, "\n\n");
        g_string_append (s, gr_recipe_get_ingredients (printer->recipe));
        g_string_append (s, "\n\n");

        attr = pango_attr_font_desc_new (title_font);
        attr->start_index = s->len;

        g_string_append (s, "Instructions");

        attr->end_index = s->len + 1;
        pango_attr_list_insert (attrs, attr);

        g_string_append (s, "\n\n");
        g_string_append (s, gr_recipe_get_instructions (printer->recipe));

        pango_layout_set_text (printer->layout, s->str, s->len);
        pango_layout_set_attributes (printer->layout, attrs);
        pango_attr_list_unref (attrs);

        gtk_print_operation_set_n_pages (operation, 1);

        pango_font_description_free (title_font);
        pango_font_description_free (body_font);
}

static void
end_print (GtkPrintOperation *operation,
             GtkPrintContext   *context,
             GrRecipePrinter   *printer)
{
        g_print ("end_print\n");
}

static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext   *context,
           int                page_nr,
           GrRecipePrinter   *printer)
{
        cairo_t *cr;
        PangoRectangle logical_rect;
        int baseline;
        int start_pos;

        g_print ("draw_page %d\n", page_nr);

        cr = gtk_print_context_get_cairo_context (context);

        cairo_set_source_rgb (cr, 0, 0, 0);

        pango_layout_get_extents (printer->layout, NULL, &logical_rect);
        baseline = pango_layout_get_baseline (printer->layout);

        cairo_move_to (cr, logical_rect.x / 1024.0, baseline / 1024.0 - logical_rect.y / 1024.0);
        pango_cairo_show_layout (cr, printer->layout);
}

static void
print_done (GtkPrintOperation       *operation,
            GtkPrintOperationResult  res,
            GrRecipePrinter         *printer)
{
       GError *error = NULL;

       g_print ("print_done\n");


  if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
    {

      GtkWidget *error_dialog;

      gtk_print_operation_get_error (operation, &error);

      error_dialog = gtk_message_dialog_new (GTK_WINDOW (printer->window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_CLOSE,
                                             "Error printing file:\n%s",
                                             error ? error->message : "no details");
      g_signal_connect (error_dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
      gtk_widget_show (error_dialog);
    }
  else if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
    {
            /* TODO: print settings */
    }

  if (!gtk_print_operation_is_finished (operation))
    {
            /* TODO monitoring */
    }

}

void
gr_recipe_printer_print (GrRecipePrinter *printer,
                         GrRecipe        *recipe)
{
        printer->recipe = g_object_ref (recipe);

        printer->print = gtk_print_operation_new ();

        /* TODO: print settings */
        /* TODO: page setup */

        g_signal_connect (printer->print, "begin-print", G_CALLBACK (begin_print), printer);
        g_signal_connect (printer->print, "end-print", G_CALLBACK (end_print), printer);
        g_signal_connect (printer->print, "draw-page", G_CALLBACK (draw_page), printer);
        g_signal_connect (printer->print, "done", G_CALLBACK (print_done), printer);

        gtk_print_operation_set_export_filename (printer->print, "recipe.pdf");

        gtk_print_operation_set_allow_async (printer->print, TRUE);

        gtk_print_operation_run (printer->print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, printer->window, NULL);
}
                                
