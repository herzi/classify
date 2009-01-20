/* vim:set et sw=2: */
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

#include <glib.h>
#include <gio/gio.h>

#include <glib/gi18n.h>

int
main (int   argc,
      char**argv)
{
  GOptionContext* options;
  GError        * error = NULL;
  gchar         **files = NULL;
  gchar         * const * file;
  GOptionEntry    entries[] = {
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &files, NULL, NULL},
    {NULL}
  };

  options = g_option_context_new (_("FILE [FILES]"));
  g_option_context_add_main_entries (options, entries, NULL);
  if (!g_option_context_parse (options, &argc, &argv, &error))
    {
      gchar* help = g_option_context_get_help (options, TRUE, NULL);
      g_printerr ("%s%s\n", help, error ? error->message : "");
      g_option_context_free (options);
      g_clear_error (&error);
      g_free (help);
      return 1;
    }

  if (!files)
    {
      gchar* help = g_option_context_get_help (options, TRUE, NULL);
      g_printerr ("%s%s\n", help, _("Missing filename argument"));
      g_option_context_free (options);
      g_free (help);
      return 2;
    }

  g_option_context_free (options);
  g_type_init ();

  for (file = files; *file; file++)
    {
      GFile* gfile = g_file_new_for_commandline_arg (*file);
      gchar* path = g_file_get_path (gfile);

      if (!path)
        {
          gchar* uri = g_file_get_uri (gfile);
          g_warning (_("Cannot handle non-local file: %s"),
                     uri);
          g_free (uri);
          g_object_unref (gfile);
          continue;
        }
      g_printf ("%s\n", path);
      g_free (path);
    }

  g_strfreev (files);
  return 0;
}

