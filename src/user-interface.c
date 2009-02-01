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

#include "marshal.h"

enum {
  CREATE_MAIN_WINDOW,
  N_SIGNALS
};

static guint ui_signals[N_SIGNALS] = {0};

G_DEFINE_TYPE (CUserInterface, c_user_interface, G_TYPE_TYPE_MODULE);

static void
c_user_interface_init (CUserInterface* self)
{}

static gboolean
ui_load (GTypeModule* module)
{
  return TRUE;
}

static void
ui_unload (GTypeModule* module)
{}

static void
c_user_interface_class_init (CUserInterfaceClass* self_class)
{
  GTypeModuleClass* module_class = G_TYPE_MODULE_CLASS (self_class);

  module_class->load   = ui_load;
  module_class->unload = ui_unload;

  ui_signals[CREATE_MAIN_WINDOW] = g_signal_new ("create-main-window",
                                                 G_OBJECT_CLASS_TYPE (self_class),
                                                 G_SIGNAL_ACTION, 0,
                                                 NULL, NULL, /* FIXME: add accumulator to only receive one */
                                                 classify_marshal_OBJECT__VOID,
                                                 GTK_TYPE_WIDGET,
                                                 0);
}

CUserInterface*
c_user_interface_new (void)
{
  return g_object_new (C_TYPE_USER_INTERFACE,
                       NULL);
}

GtkWidget*
c_user_interface_get_main_window (CUserInterface* self)
{
  static gpointer main_window = NULL;

  g_return_val_if_fail (C_IS_USER_INTERFACE (self), NULL);

  if (G_UNLIKELY (!main_window))
    {
      g_signal_emit (self, ui_signals[CREATE_MAIN_WINDOW], 0, &main_window);

      if (!main_window)
        {
          g_warning ("no window was created: exit");
          exit (1);
        }

      g_object_add_weak_pointer (main_window, &main_window);
    }
  else
    {
      g_object_ref_sink (main_window);
    }

  return main_window;
}

