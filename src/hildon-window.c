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

static void     c_hildon_window_init        (CHildonWindow     * self);
static void     c_hildon_window_class_init  (CHildonWindowClass* self_class);

static GType c_hildon_window_type = 0;

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

static void
c_hildon_window_class_init (CHildonWindowClass* self_class)
{}

GtkWidget*
c_hildon_window_new (void)
{
  return g_object_new (C_TYPE_HILDON_WINDOW,
                       NULL);
}
