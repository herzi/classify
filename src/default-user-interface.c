/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <sven@imendio.com>
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

#include "user-interface.h"

#include "default-window.h"

G_MODULE_EXPORT GtkWidget* c_user_interface_module_create_window (void);
G_MODULE_EXPORT gint       c_ui_module_get_priority              (void);
G_MODULE_EXPORT void       c_ui_module_register_types            (GTypeModule* module);

GtkWidget*
c_user_interface_module_create_window (void)
{
  return c_default_window_new ();
}

gint
c_ui_module_get_priority (void)
{
  return 0; /* default priority */
}

void
c_ui_module_register_types (GTypeModule* module)
{
  c_default_window_register_type (module);
}

