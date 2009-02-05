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

GType
c_main_window_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    {
      GTypeInfo const info = {
        sizeof (CMainWindowIface),
        NULL, NULL,
        NULL, NULL, NULL,
        0, 0,
        NULL
      };

      type = g_type_register_static (G_TYPE_INTERFACE,
                                     G_STRINGIFY (CMainWindow),
                                     &info, 0);
    }

  return type;
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

