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

#include "user-interface-factory.h"

static int
compare_uis (gconstpointer a,
             gconstpointer b,
             gpointer      user_data G_GNUC_UNUSED)
{
  return c_user_interface_get_priority (b) - c_user_interface_get_priority (a);
}

static void
queue_fill (GQueue     * queue,
            gchar const* path)
{
  GError     * error = NULL;
  GDir       * dir = g_dir_open (path, 0, &error);
  gchar const* file;

  if (error && error->domain == G_FILE_ERROR && error->code == G_FILE_ERROR_NOENT)
    {
      g_error_free (error);
      return;
    }
  else if (error)
    {
      g_warning ("error opening module folder: %s", error->message);
      g_error_free (error);
      return;
    }

  for (file = g_dir_read_name (dir); file; file = g_dir_read_name (dir))
    {
      CUserInterface* ui;
      gchar         * filename;

      if (!g_str_has_suffix (file, "." G_MODULE_SUFFIX))
        {
          continue;
        }

      filename = g_build_filename (path, file, NULL);
      ui       = c_user_interface_new (filename);

      if (c_user_interface_is_valid (ui))
        {
          /* FIXME: introduce priorities */
          g_queue_insert_sorted (queue, ui, compare_uis, NULL);
        }
      else
        {
          g_object_unref (ui);
        }

      g_free (filename);
    }

  g_dir_close (dir);
}

CUserInterface*
c_user_interface_factory_get_ui (void)
{
  static GQueue* queue = NULL;

  if (G_UNLIKELY (!queue))
    {
      gchar const* classify_modules = g_getenv ("CLASSIFY_MODULES");

      queue = g_queue_new ();

      if (classify_modules)
        {
          gchar**paths = g_strsplit (classify_modules, G_SEARCHPATH_SEPARATOR_S, -1);
          gchar**path;

          for (path = paths; *path; path++)
            {
              queue_fill (queue, *path);
            }

          g_strfreev (paths);
        }

      queue_fill (queue, MOD_DIR);
    }

  return queue->head ? queue->head->data : NULL;
}

