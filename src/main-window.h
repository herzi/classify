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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _CMainWindow      CMainWindow;
typedef struct _CMainWindowIface CMainWindowIface;

#define C_TYPE_MAIN_WINDOW         (c_main_window_get_type ())
#define C_MAIN_WINDOW(i)           (G_TYPE_CHECK_INSTANCE_CAST ((i), C_TYPE_MAIN_WINDOW, CMainWindow))
#define C_IS_MAIN_WINDOW(i)        (G_TYPE_CHECK_INSTANCE_TYPE ((i), C_TYPE_MAIN_WINDOW))
#define C_MAIN_WINDOW_GET_IFACE(i) (G_TYPE_INSTANCE_GET_INTERFACE ((i), C_TYPE_MAIN_WINDOW, CMainWindowIface))

GType         c_main_window_get_type (void);

void          c_main_window_implement    (GObjectClass* object_class,
                                          guint         prop_ui_manager);
void          c_main_window_constructed  (CMainWindow * self);
void          c_main_window_initialize   (CMainWindow * self);

GtkWidget*    c_main_window_get_content  (CMainWindow * self);
GtkMenuShell* c_main_window_get_menus    (CMainWindow * self);
GtkToolbar*   c_main_window_get_toolbar  (CMainWindow * self);

void          c_main_window_pack_content (CMainWindow * self,
                                          GtkWidget   * content);
void          c_main_window_pack_menus   (CMainWindow * self,
                                          GtkMenuShell* menus);
void          c_main_window_pack_tools   (CMainWindow * self,
                                          GtkToolbar  * tools);

struct _CMainWindowIface {
  GTypeInterface base_interface;

  /* vtable */
  GtkWidget*    (*get_content)  (CMainWindow * self);
  GtkMenuShell* (*get_menus)    (CMainWindow * self);
  GtkToolbar*   (*get_toolbar)  (CMainWindow * self);

  void          (*pack_content) (CMainWindow * self,
                                 GtkWidget   * content);
  void          (*pack_menus)   (CMainWindow * self,
                                 GtkMenuShell* menus);
  void          (*pack_tools)   (CMainWindow * self,
                                 GtkToolbar  * tools);
};

G_END_DECLS

#endif /* !MAIN_WINDOW_H */
