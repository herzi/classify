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

#include "main-window.h"

#define GETTEXT_PACKAGE NULL
#include <glib/gi18n-lib.h>

static void
c_main_window_iface_init (gpointer iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("ui-manager",
                                                            "ui manager",
                                                            "the ui manager of this window",
                                                            GTK_TYPE_UI_MANAGER,
                                                            G_PARAM_READWRITE));
}

GType
c_main_window_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    {
      GTypeInfo const info = {
        sizeof (CMainWindowIface),
        NULL, NULL,
        (GClassInitFunc) c_main_window_iface_init,
        NULL, NULL,
        0, 0,
        NULL
      };

      type = g_type_register_static (G_TYPE_INTERFACE,
                                     G_STRINGIFY (CMainWindow),
                                     &info, 0);

      g_type_interface_add_prerequisite (type, GTK_TYPE_WINDOW);
    }

  return type;
}

void
c_main_window_constructed (CMainWindow* self)
{
  g_return_if_fail (C_IS_MAIN_WINDOW (self));

  c_main_window_pack_menus (self, c_main_window_get_menus (self));
  c_main_window_pack_tools (self, c_main_window_get_toolbar (self));

  c_main_window_pack_content (self, c_main_window_get_content (self));
}

static void
view_expand_all_activated (GtkAction  * action,
                           CMainWindow* self)
{
  gtk_tree_view_expand_all (GTK_TREE_VIEW (c_main_window_get_content (self)));
}

static void
view_collapse_all_activated (GtkAction  * action,
                             CMainWindow* self)
{
  gtk_tree_view_collapse_all (GTK_TREE_VIEW (c_main_window_get_content (self)));
}

void
c_main_window_initialize (CMainWindow * self)
{
  GtkActionGroup* actions;
  GtkUIManager  * ui_manager;
  GtkActionEntry  entries[] = {
		{"File", NULL, N_("_File")},
		{"Edit", NULL, N_("_Edit")},

		{"View", NULL, N_("_View")},
		{"ViewExpandAll", NULL, N_("_Expand All"),
		 NULL, NULL, // FIXME: add tooltip
		 G_CALLBACK (view_expand_all_activated)},
		{"ViewCollapseAll", NULL, N_("_Collapse All"),
		 NULL, NULL, // FIXME: add tooltip
                 G_CALLBACK (view_collapse_all_activated)}
  };

  g_return_if_fail (C_IS_MAIN_WINDOW (self));

  ui_manager = gtk_ui_manager_new ();
  g_object_set (self,
                "ui-manager", ui_manager,
                NULL);

  gtk_window_set_default_size (GTK_WINDOW (self),
                               400, 300);
  gtk_window_set_title        (GTK_WINDOW (self),
                               _("List of Tasks"));

  gtk_window_add_accel_group  (GTK_WINDOW (self),
                               gtk_ui_manager_get_accel_group (ui_manager));

  actions = gtk_action_group_new (g_type_name (C_TYPE_MAIN_WINDOW));
  gtk_action_group_add_actions (actions, entries, G_N_ELEMENTS (entries), self);
  gtk_ui_manager_insert_action_group (ui_manager, actions, -1);
  g_object_unref (actions);

  g_object_unref (ui_manager);
}

void
c_main_window_implement (GObjectClass* object_class,
                         guint         prop_ui_manager)
{
  g_object_class_override_property (object_class, prop_ui_manager, "ui-manager");
}

GtkWidget*
c_main_window_get_content (CMainWindow* self)
{
  g_return_val_if_fail (C_IS_MAIN_WINDOW (self), NULL);

  if (C_MAIN_WINDOW_GET_IFACE (self)->get_content)
    {
      return C_MAIN_WINDOW_GET_IFACE (self)->get_content (self);
    }
  else
    {
      g_warning ("%s doesn't implement get_content()",
                 G_OBJECT_TYPE_NAME (self));
      return NULL;
    }
}

GtkMenuShell*
c_main_window_get_menus (CMainWindow* self)
{
  g_return_val_if_fail (C_IS_MAIN_WINDOW (self), NULL);

  if (C_MAIN_WINDOW_GET_IFACE (self)->get_menus)
    {
      return C_MAIN_WINDOW_GET_IFACE (self)->get_menus (self);
    }
  else
    {
      g_warning ("%s doesn't implement get_menus()",
                 G_OBJECT_TYPE_NAME (self));
      return NULL;
    }
}

GtkToolbar*
c_main_window_get_toolbar (CMainWindow * self)
{
  g_return_val_if_fail (C_IS_MAIN_WINDOW (self), NULL);

  if (C_MAIN_WINDOW_GET_IFACE (self)->get_toolbar)
    {
      return C_MAIN_WINDOW_GET_IFACE (self)->get_toolbar (self);
    }
  else
    {
      g_warning ("%s doesn't implement get_toolbar()",
                 G_OBJECT_TYPE_NAME (self));
      return NULL;
    }
}

void
c_main_window_pack_content (CMainWindow * self,
                            GtkWidget   * content)
{
  g_return_if_fail (C_IS_MAIN_WINDOW (self));
  g_return_if_fail (GTK_IS_WIDGET (content));

  if (C_MAIN_WINDOW_GET_IFACE (self)->pack_content)
    {
      C_MAIN_WINDOW_GET_IFACE (self)->pack_content (self, content);
    }
  else
    {
      g_warning ("%s doesn't implement pack_content()",
                 G_OBJECT_TYPE_NAME (self));
    }
}

void
c_main_window_pack_menus (CMainWindow * self,
                          GtkMenuShell* menus)
{
  g_return_if_fail (C_IS_MAIN_WINDOW (self));
  g_return_if_fail (GTK_IS_MENU_SHELL (menus));

  if (C_MAIN_WINDOW_GET_IFACE (self)->pack_menus)
    {
      C_MAIN_WINDOW_GET_IFACE (self)->pack_menus (self, menus);
    }
  else
    {
      g_warning ("%s doesn't implement pack_menus()",
                 G_OBJECT_TYPE_NAME (self));
    }
}

void
c_main_window_pack_tools (CMainWindow * self,
                          GtkToolbar  * tools)
{
  g_return_if_fail (C_IS_MAIN_WINDOW (self));
  g_return_if_fail (GTK_IS_TOOLBAR (tools));

  if (C_MAIN_WINDOW_GET_IFACE (self)->pack_tools)
    {
      C_MAIN_WINDOW_GET_IFACE (self)->pack_tools (self, tools);
    }
  else
    {
      g_warning ("%s doesn't implement pack_tools()",
                 G_OBJECT_TYPE_NAME (self));
    }
}

