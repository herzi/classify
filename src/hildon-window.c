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
view_fullscreen (GtkAction* action,
                 GtkWidget* widget)
{
  if ((gdk_window_get_state (widget->window) & GDK_WINDOW_STATE_FULLSCREEN) == 0)
    {
      gtk_window_fullscreen (GTK_WINDOW (widget));
    }
  else
    {
      gtk_window_unfullscreen (GTK_WINDOW (widget));
    }
}

static void
c_hildon_window_init (CHildonWindow* self)
{
  GtkActionGroup* group;
  GtkActionEntry  entries[] = {
                {"ViewToggleFullscreen", GTK_STOCK_FULLSCREEN, NULL,
                 "F6", NULL, // FIXME: add tooltip
                 G_CALLBACK (view_fullscreen)}
  };
  GtkStockItem    item;
  GError        * error = NULL;
  GList         * groups;
  GList         * iter;

  group = gtk_action_group_new ("backend-actions");
  gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), self);
  gtk_ui_manager_insert_action_group (c_window_get_ui_manager (C_WINDOW (self)), group, -1);
  g_object_unref (group);

  if (gtk_stock_lookup (GTK_STOCK_FULLSCREEN, &item))
    {
      g_object_set (gtk_action_group_get_action (group, "ViewToggleFullscreen"),
                    "icon-name", "qgn_list_hw_button_view_toggle",
                    "label", g_dgettext (item.translation_domain, item.label),
                    "stock-id", NULL,
                    NULL);
    }

  groups = gtk_ui_manager_get_action_groups (c_window_get_ui_manager (C_WINDOW (self)));
  for (iter = groups; iter; iter = iter->next)
    {
      if (iter->data != group)
        {
          group = iter->data;
          break;
        }
    }

        if (gtk_stock_lookup (GTK_STOCK_ADD, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskNew"),
                          "icon-name", "qgn_indi_gene_plus",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_GOTO_BOTTOM, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskBottom"),
                          "icon-name", "qgn_indi_arrow_down",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_GOTO_TOP, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "TaskTop"),
                          "icon-name", "qgn_indi_arrow_up",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_COPY, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditCopy"),
                          "icon-name", "qgn_list_gene_copy",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_PASTE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditPaste"),
                          "icon-name", "qgn_list_gene_paste",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_DELETE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "EditDelete"),
                          "icon-name", "qgn_toolb_gene_deletebutton",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }
        if (gtk_stock_lookup (GTK_STOCK_CLOSE, &item))
          {
            g_object_set (gtk_action_group_get_action (group, "FileClose"),
                          "icon-name", "qgn_toolb_gene_close",
                          "label", g_dgettext (item.translation_domain, item.label),
                          "stock-id", NULL,
                          NULL);
          }

        gtk_ui_manager_add_ui_from_string  (c_window_get_ui_manager (C_WINDOW (self)),
                                            "<ui>"
                                              "<popup name='menus'>"
                                                "<menuitem action='TaskNew'/>"
                                                "<separator/>"
                                                "<menu action='Edit'>"
                                                  "<menuitem action='EditCopy'/>"
                                                  "<menuitem action='EditPaste'/>"
                                                  "<menuitem action='EditDelete'/>"
                                                  "<separator/>"
                                                /*  "<menuitem action='EditRename'/>" */ // FIXME: doesn't work yet
                                                "</menu>"
                                                "<separator/>"
                                                "<menuitem action='ViewToggleFullscreen'/>"
                                                "<menuitem action='ViewExpandAll'/>"
                                                "<menuitem action='ViewCollapseAll'/>"
                                                "<separator/>"
                                                "<menuitem action='FileClose' />"
					      "</popup>"
					    "</ui>",
					    -1,
					    &error);
}

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
hildon_window_pack_content (CWindow  * window,
                            GtkWidget* content)
{
  hildon_window_add_with_scrollbar (HILDON_WINDOW (window), content);

  gtk_widget_show_all (GTK_WIDGET (window));
}

static void
hildon_window_pack_menu_shell (CWindow     * window,
                               GtkMenuShell* menus)
{
  hildon_window_set_menu (HILDON_WINDOW (window), GTK_MENU (menus));
}

static void
hildon_window_pack_toolbar (CWindow   * window,
                            GtkToolbar* toolbar)
{
  if (C_WINDOW_CLASS (c_hildon_window_parent_class)->pack_toolbar)
    {
      C_WINDOW_CLASS (c_hildon_window_parent_class)->pack_toolbar (window, toolbar);
    }

  hildon_window_add_toolbar (HILDON_WINDOW (window), toolbar);
}

static void
c_hildon_window_class_init (CHildonWindowClass* self_class)
{
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);
  CWindowClass  * window_class = C_WINDOW_CLASS (self_class);

  c_hildon_window_parent_class = g_type_class_peek_parent (self_class);

  widget_class->window_state_event = window_state_event;

  window_class->pack_content       = hildon_window_pack_content;
  window_class->pack_menu_shell    = hildon_window_pack_menu_shell;
  window_class->pack_toolbar       = hildon_window_pack_toolbar;
}

GtkWidget*
c_hildon_window_new (void)
{
  return g_object_new (C_TYPE_HILDON_WINDOW,
                       NULL);
}

