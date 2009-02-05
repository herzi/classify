/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@lanedo.com>
 *
 * Copyright (C) 2009  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "hildon-window.h"

/* FIXME: define a proper package */
#define GETTEXT_PACKAGE NULL
#include <glib/gi18n-lib.h>

#if !GLIB_CHECK_VERSION(2,18,0)
#define g_dgettext(dom,msg) dgettext (dom, msg)
#endif

static void     c_hildon_window_init        (CHildonWindow     * self);
static void     c_hildon_window_class_init  (CHildonWindowClass* self_class);

static GType    c_hildon_window_type = 0;
static gpointer c_hildon_window_parent_class = NULL;

GType
c_hildon_window_register_type (GTypeModule* module)
{
  if (G_UNLIKELY (!c_hildon_window_type))
    {
      GTypeInfo const info = {
        sizeof (CWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_hildon_window_class_init,
        NULL, NULL,
        sizeof (CWindow), 0,
        (GInstanceInitFunc) c_hildon_window_init,
        NULL
      };
      c_hildon_window_type = g_type_module_register_type (module,
                                                          c_ui_module_register_type (module),
                                                          G_STRINGIFY (CHildonWindow),
                                                          &info,
                                                          0);
    }

  return c_hildon_window_type;
}

GType
c_hildon_window_get_type (void)
{
  return c_hildon_window_type;
}

static void
c_hildon_window_init (CHildonWindow* self)
{}

static gboolean
window_state_event (GtkWidget          * widget,
                    GdkEventWindowState* event)
{
  if ((event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN) != 0)
    {
      GtkStockItem  item;
      gchar const * stock_id = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) ? GTK_STOCK_LEAVE_FULLSCREEN : GTK_STOCK_FULLSCREEN;
      GtkAction   * action = gtk_ui_manager_get_action (c_window_get_ui_manager (C_WINDOW (widget)),
                                                        "/menus/ViewToggleFullscreen");

      if (gtk_stock_lookup (stock_id, &item))
        {
          g_object_set (action,
                        "icon-name", "qgn_list_hw_button_view_toggle",
                        "label", g_dgettext (item.translation_domain, item.label),
                        "stock-id", NULL,
                        NULL);
        }
      else
        {
          g_object_set (action,
                        "stock-id", stock_id,
                        NULL);
        }
    }

  if (GTK_WIDGET_CLASS (c_hildon_window_parent_class)->window_state_event)
    {
      return GTK_WIDGET_CLASS (c_hildon_window_parent_class)->window_state_event (widget, event);
    }

  return FALSE;
}

static void
hildon_window_pack_menu_shell (CWindow* self)
{
  GtkWidget* shell;

  if (C_WINDOW_CLASS (c_hildon_window_parent_class)->pack_menu_shell)
    {
      C_WINDOW_CLASS (c_hildon_window_parent_class)->pack_menu_shell (self);
    }

  shell = gtk_ui_manager_get_widget (c_window_get_ui_manager (self), "/ui/menus");

  hildon_window_set_menu (HILDON_WINDOW (self),
                          GTK_MENU (shell));
}
static void
c_hildon_window_class_init (CHildonWindowClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);
  CWindowClass  * window_class = C_WINDOW_CLASS (self_class);

  c_hildon_window_parent_class = g_type_class_peek_parent (self_class);

  widget_class->window_state_event = window_state_event;

  window_class->pack_menu_shell    = hildon_window_pack_menu_shell;
}

GtkWidget*
c_hildon_window_new (void)
{
  return g_object_new (C_TYPE_HILDON_WINDOW,
                       NULL);
}

