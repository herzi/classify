/* This file is part of classify
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2007,2009  Sven Herzberg
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

#include <gtk/gtk.h>
#include "user-interface-factory.h"

#include <glib/gi18n.h>

int
main (int   argc,
      char**argv)
{
  CUserInterface* user_interface;
  GtkWidget     * window;

  g_set_application_name (_("Classify"));
  gtk_init (&argc, &argv);

  user_interface = c_user_interface_factory_get_ui ();
  if (!user_interface || !g_type_module_use (G_TYPE_MODULE (user_interface)))
    {
      /* FIXME: popup a dialog */
      g_warning ("no suitable user interface module found"); /* FIXME: mention search paths */
      return 1;
    }

  window = c_user_interface_get_main_window (user_interface);

  gtk_widget_show (window);
  gtk_main ();

  g_type_module_unuse (G_TYPE_MODULE (user_interface));

  return 0;
}

