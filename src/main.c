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

static void
my_log_func (gchar const   * domain,
             GLogLevelFlags  flags,
             gchar const   * message,
             gpointer        user_data)
{
  GLogFunc old_func = user_data;

  if (!user_data)
    {
      old_func = g_log_default_handler;
    }

  if ((flags & G_LOG_LEVEL_MASK) <= G_LOG_LEVEL_MESSAGE)
    old_func (domain, flags, message, NULL);
}

int
main (int   argc,
      char**argv)
{
  CUserInterface* user_interface;
  GOptionContext* context;
  GtkWidget     * window;
  GError        * error = NULL;
  gboolean        verbose = FALSE;
  GOptionEntry    entries[] = {
    {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, N_("be verbose"), NULL},
    {NULL}
  };

  g_set_application_name (_("Classify"));

  context = g_option_context_new ("");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
#if GLIB_CHECK_VERSION(2,14,0)
      gchar* help = g_option_context_get_help (context, TRUE, NULL);
      g_printerr ("%s", help);
      g_free (help);

      if (error)
        {
          g_warning ("%s", error->message);
        }
#else
      g_warning ("error parsing command liner arguments: %s", error ? error->message : "no error supplied");
#endif

      g_clear_error (&error);
      g_option_context_free (context);
      return 1;
    }

  g_option_context_free (context);

  if (!verbose)
    {
      GLogFunc old_handler = g_log_set_default_handler (my_log_func, NULL);
      g_log_set_default_handler (my_log_func, old_handler);
    }

  gtk_window_set_default_icon_name ("classify");

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

