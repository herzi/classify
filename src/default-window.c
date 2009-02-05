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

#include "default-window.h"

#include "preferences.h"

struct _CDefaultWindowPrivate {
  GtkWidget* vbox;
};

#define PRIV(i) (((CDefaultWindow*)(i))->_private)

static void c_default_window_init       (CDefaultWindow     * self);
static void c_default_window_class_init (CDefaultWindowClass* self_class);

static GType    c_default_window_type = 0;
static gpointer c_default_window_parent_class = NULL;

GType
c_default_window_get_type (void)
{
  return c_default_window_type;
}

GType
c_default_window_register_type (GTypeModule* module)
{
  if (G_UNLIKELY (!c_default_window_type))
    {
      GTypeInfo const info = {
        sizeof (CDefaultWindowClass),
        NULL, NULL,
        (GClassInitFunc) c_default_window_class_init,
        NULL, NULL,
        sizeof (CDefaultWindow), 0,
        (GInstanceInitFunc) c_default_window_init,
        NULL
      };

      c_default_window_type = g_type_module_register_type (module,
                                                           c_ui_module_register_type (module),
                                                           G_STRINGIFY (CDefaultWindow),
                                                           &info,
                                                           0);
    }

  return c_default_window_type;
}

static void
c_default_window_init (CDefaultWindow* self)
{
  PRIV (self) = G_TYPE_INSTANCE_GET_PRIVATE (self, C_TYPE_DEFAULT_WINDOW, CDefaultWindowPrivate);
}

static void
open_prefs (GtkAction* action,
            CWindow  * self)
{
  GtkWidget* dialog = c_preferences_new (GTK_WINDOW (self));
  gtk_dialog_run     (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void
default_window_pack_menu_shell (CWindow* window)
{
  GtkActionGroup* group;
  GtkActionEntry  entries[] = {
		{"EditPreferences", GTK_STOCK_PREFERENCES, NULL,
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (open_prefs)}
  };
  GError        * error = NULL;

  group = gtk_action_group_new ("backend-actions");
  gtk_action_group_add_actions (group, entries, G_N_ELEMENTS (entries), window);
  gtk_ui_manager_insert_action_group (c_window_get_ui_manager (window), group, -1);
  g_object_unref (group);

        gtk_ui_manager_add_ui_from_string  (c_window_get_ui_manager (window),
                                            "<ui>"
                                                "<menubar name='menus'>"
                                                        "<menu action='File'>"
                                                                "<menuitem action='TaskNew'/>"
								"<separator/>"
								"<menuitem action='FileClose' />"
							"</menu>"
							"<menu action='Edit'>"
								"<menuitem action='EditCopy'/>"
								"<menuitem action='EditPaste'/>"
								"<menuitem action='EditDelete'/>"
								"<separator/>"
								"<menuitem action='EditRename'/>"
								"<separator/>"
								"<menuitem action='EditPreferences' />"
							"</menu>"
							"<menu action='View'>"
								"<menuitem action='ViewExpandAll'/>"
								"<menuitem action='ViewCollapseAll'/>"
							"</menu>"
						"</menubar>"
					    "</ui>",
					    -1,
					    &error);

  if (error)
    {
      g_warning ("error constructing window: %s", error->message);
      g_error_free (error);
      return;
    }

  if (C_WINDOW_CLASS (c_default_window_parent_class)->pack_menu_shell)
    {
      C_WINDOW_CLASS (c_default_window_parent_class)->pack_menu_shell (window);
    }
}

static void
c_default_window_class_init (CDefaultWindowClass* self_class)
{
  CWindowClass* window_class = C_WINDOW_CLASS (self_class);

  c_default_window_parent_class = g_type_class_peek_parent (self_class);

  window_class->pack_menu_shell = default_window_pack_menu_shell;

  g_type_class_add_private (self_class, sizeof (CDefaultWindowPrivate));
}

GtkWidget*
c_default_window_new (void)
{
  return g_object_new (C_TYPE_DEFAULT_WINDOW,
                       NULL);
}

