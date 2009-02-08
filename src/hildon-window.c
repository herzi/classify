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

#include "main-window.h"

/* FIXME: define a proper package */
#define GETTEXT_PACKAGE NULL
#include <glib/gi18n-lib.h>

struct _CHildonWindowPrivate {
  GtkWidget* content;
};

#define PRIV(i) (((CHildonWindow*)(i))->_private)

#if !GLIB_CHECK_VERSION(2,18,0)
#define g_dgettext(dom,msg) dgettext (dom, msg)
#endif

static void     c_hildon_window_init        (CHildonWindow     * self);
static void     c_hildon_window_class_init  (CHildonWindowClass* self_class);
static void     implement_main_window       (CMainWindowIface  * iface);

static GType    c_hildon_window_type = 0;
static gpointer c_hildon_window_parent_class = NULL;

GType
c_hildon_window_register_type (GTypeModule* module)
{
  if (G_UNLIKELY (!c_hildon_window_type))
    {
      GTypeInfo const info = {
        sizeof (CHildonWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_hildon_window_class_init,
        NULL, NULL,
        sizeof (CHildonWindow), 0,
        (GInstanceInitFunc) c_hildon_window_init,
        NULL
      };
      GInterfaceInfo const iinfo = {
        (GInterfaceInitFunc) implement_main_window,
        NULL, NULL
      };
      c_hildon_window_type = g_type_module_register_type (module,
                                                          c_ui_module_register_type (module),
                                                          G_STRINGIFY (CHildonWindow),
                                                          &info,
                                                          0);
      g_message ("begin: if you see a warning after this...");
      g_type_module_add_interface (module,
                                   c_hildon_window_type,
                                   C_TYPE_MAIN_WINDOW,
                                   &iinfo);
      g_message ("end:   ...and before this, you need the patches from http://bugzilla.gnome.org/show_bug.cgi?id=570826");
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
  GHashTable    * hildon_icons = g_hash_table_new (g_str_hash, g_str_equal);
  GError        * error = NULL;
  GList         * groups;
  GList         * iter;

  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_HILDON_WINDOW, CHildonWindowPrivate);

  g_hash_table_insert (hildon_icons,
                       "ViewToggleFullscreen",
                       "qgn_list_hw_button_view_toggle");
  g_hash_table_insert (hildon_icons,
                       "TaskNew",
                       "qgn_indi_gene_plus");
  g_hash_table_insert (hildon_icons,
                       "TaskBottom",
                       "qgn_indi_arrow_down");
  g_hash_table_insert (hildon_icons,
                       "TaskTop",
                       "qgn_indi_arrow_up");
  g_hash_table_insert (hildon_icons,
                       "EditCopy",
                       "qgn_list_gene_copy");
  g_hash_table_insert (hildon_icons,
                       "EditPaste",
                       "qgn_list_gene_paste");
  g_hash_table_insert (hildon_icons,
                       "EditDelete",
                       "qgn_toolb_gene_deletebutton");
  g_hash_table_insert (hildon_icons,
                       "FileClose",
                       "qgn_toolb_gene_close");

  group = gtk_action_group_new ("backend-actions");
  gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), self);
  gtk_ui_manager_insert_action_group (c_window_get_ui_manager (C_WINDOW (self)), group, -1);

  groups = gtk_ui_manager_get_action_groups (c_window_get_ui_manager (C_WINDOW (self)));
  for (iter = groups; iter; iter = iter->next)
    {
      GList* actions = gtk_action_group_list_actions (iter->data);
      GList* action;

      for (action = actions; action; action = action->next)
        {
          gchar const* icon = g_hash_table_lookup (hildon_icons, gtk_action_get_name (action->data));
          gchar      * stock_id = NULL;

          if (!icon)
            {
              continue;
            }

          g_object_get (action->data,
                        "stock-id", &stock_id,
                        NULL);

          if (stock_id && gtk_stock_lookup (stock_id, &item))
            {
              g_object_set (action->data,
                            "label", g_dgettext (item.translation_domain, item.label),
                            "stock-id", NULL,
                            NULL);
            }

          g_object_set (action->data,
                        "icon-name", icon,
                        NULL);

          g_free (stock_id);
        }

      g_list_free (actions);
    }
  g_object_unref (group);
  g_hash_table_destroy (hildon_icons);

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

  PRIV (window)->content = content;

  /* FIXME: debug, bug-report, fix and workaround */
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

  g_type_class_add_private (self_class, sizeof (CHildonWindowPrivate));
}

GtkWidget*
c_hildon_window_new (void)
{
  return g_object_new (C_TYPE_HILDON_WINDOW,
                       NULL);
}

static GtkWidget*
hildon_window_get_content (CMainWindow* main_window)
{
  return PRIV (main_window)->content;
}

static void
implement_main_window (CMainWindowIface* iface)
{
  iface->get_content = hildon_window_get_content;
}

