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

static gboolean
load_module (CUserInterface* self,
             gchar const   * path)
{
  GModule* module = g_module_open (path, G_MODULE_BIND_LAZY);

  if (!module)
    {
      g_warning ("error opening module \"%s\"", path);
      return FALSE;
    }

  g_object_set_data (G_OBJECT (self),
                     "CUserInterface::Module",
                     module);

  return TRUE;
}

static void
unload_module (CUserInterface* self,
               gchar const   * path)
{
  GModule* module = g_object_steal_data (G_OBJECT (self), "CUserInterface::Module");

  if (!g_module_close (module))
    {
      g_warning ("error closing module \"%s\"", path);
    }
}

static GtkWidget*
create_main_window (CUserInterface* self)
{
  GModule* module = g_object_get_data (G_OBJECT (self), "CUserInterface::Module");
  GtkWidget* (*ui_new) (void);

  if (!g_module_symbol (module, "c_user_interface_module_create_window", (gpointer*)&ui_new))
    {
      g_warning ("error looking up \"c_user_interface_module_create_window()\"");
      return;
    }

  g_return_val_if_fail (ui_new != NULL, NULL);

  return ui_new ();
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
      gchar* filename = g_build_filename (path, file, NULL);
      GModule* module = g_module_open (filename, G_MODULE_BIND_LAZY);
      gpointer ui_new;

      g_free (filename);

      if (!module)
        continue;

      if (g_module_symbol (module, "c_user_interface_module_create_window", (gpointer*)&ui_new))
        {
          /* now we have a valid module */
          CUserInterface* ui = c_user_interface_new ();
          filename = g_build_filename (path, file, NULL);
          g_signal_connect (ui, "load",
                            G_CALLBACK (load_module), filename);
          g_signal_connect (ui, "unload",
                            G_CALLBACK (unload_module), NULL);
          g_signal_connect (ui, "create-main-window",
                            G_CALLBACK (create_main_window), NULL);

          /* FIXME: introduce priorities */
          g_queue_push_head (queue, ui);
        }

      if (!g_module_close (module))
        g_warning ("error closing module \"%s%c%s\"",
                   path,
                   G_DIR_SEPARATOR,
                   file);
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

